#include "Server.hpp"
#include "Request.hpp"
#include "Logger.hpp"
#include "Error.hpp"

#include <algorithm>
#include <system_error>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EVENTS 30
#define REQUEST_BUFFER_SIZE 5000

using namespace server::network;

static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) throw std::runtime_error("fcntl F_GETFL");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) 
        throw std::runtime_error("fcntl F_SETFL");
}

Server::Server(uint32_t threads): state{ServerState::STARTING}, tp(threads) {}
Server::~Server() {}

void Server::listenPort(std::string port){

    LOG_INFO << "Server starting on port " << port;

    addrinfo hint, *res, *p; 
    epoll_event ev, events[MAX_EVENTS];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(nullptr, port.c_str(), &hint, &res) != 0) 
        throw std::runtime_error(strerror(errno));
    
    for (p = res; p != nullptr; p = p->ai_next){
        serverfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (serverfd == -1) {
            continue;
        }

        int yes = 1;
        setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        int err = bind(serverfd, p->ai_addr, p->ai_addrlen);
        if (err != 0) {
            close(serverfd);
            continue;
        }

        LOG_DEBUG << "Server socket bound to port and IP address.";

        break;
    }

    freeaddrinfo(res);

    if (p == nullptr)
        throw std::runtime_error("error opening socket or binding");

    setNonBlocking(serverfd);

    int err = listen(serverfd, 20);
    if (err != 0)
        throw std::runtime_error(strerror(errno));
    
    epollfd = epoll_create1(0);
    if (epollfd == -1)
        throw std::runtime_error(strerror(errno));

    ev.events = EPOLLIN;
    ev.data.fd = serverfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverfd, &ev) == -1)
        throw std::runtime_error(strerror(errno));

    state = ServerState::LISTENING;

    LOG_INFO << "Server is listening";

    for (;;) {
        int eventn = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (eventn == -1) {
            if (errno == EINTR) continue;
            throw std::runtime_error(strerror(errno));
        }

        for (int i = 0; i < eventn; ++i){
            epoll_event event = events[i];
            if (event.data.fd == serverfd){
                acceptConnection();
            }else {
                handleRequest(event.data.fd);
            }
        }
    }
}

void Server::acceptConnection(){
    int fd = accept(serverfd, nullptr, 0);
    if (fd == -1){
        return; 
    }

    LOG_DEBUG << "Connection accepted with fd " << fd;

    try {
        setNonBlocking(fd);
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to set non-blocking on fd " << fd << ": " << e.what();
        close(fd);
        return;
    }

    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLONESHOT;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1){
        close(fd);
        return;
    }

    std::lock_guard<std::mutex> lk(mu);
    connections.emplace(std::piecewise_construct, 
                        std::forward_as_tuple(fd), 
                        std::forward_as_tuple(fd));
}

void Server::handleRequest(int fd){ 
    LOG_DEBUG << "Data coming from connection in fd " << fd;

    tp.submit([this, fd]{
        Connection* conn_ptr = nullptr;
        {
            std::lock_guard<std::mutex> lk(mu);
            auto it = connections.find(fd);
            if (it == connections.end()) return;
            conn_ptr = &it->second;
        }

        Connection& conn = *conn_ptr;
        bool should_close = false;

        try {
            std::optional<Request> requestopt = conn.parseRequest();
            if (!requestopt.has_value()) {
                // Not done parsing, re-arm epoll for this fd
                epoll_event ev;
                ev.events = EPOLLIN | EPOLLONESHOT;
                ev.data.fd = fd;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
                return;
            }
            
            Request req = requestopt.value();
            RadixQueryResult queryResponse = radixTree.query(
                req.method, req.path.URL);
            ContextBuilder builder;

            Response res;

            Context context = builder
                .setParams(queryResponse.params)
                .setFilters(queryResponse.filter.root)
                .setRequest(req)
                .setRespose(res)
                .getContext();

            context.start();

            conn.writeResponse(context.res.encode());
            
            should_close = true;

        } catch(const HttpError& e){
            LOG_ERROR << "http error in connection " << fd << ": " << e.getPayload();
            try {
                Response response(e.getStatus(), e.getPayload());
                conn.writeResponse(response.encode());
            } catch (...) {}
            should_close = true;
        } catch (const std::exception& e) {
            LOG_ERROR << "runtime error in connection " << fd << ": " << e.what();
            try {
                Response response(Status::InternalServerError, "Internal server error");
                conn.writeResponse(response.encode());
            } catch (...) {}
            should_close = true;
        } catch (...) {
            LOG_ERROR << "unexpected error in connection " << fd;
            should_close = true;
        }

        if (should_close) {
            closeConnection(fd);
        }
    });
}

void Server::closeConnection(int fd){
    std::lock_guard<std::mutex> lk(mu);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
    connections.erase(fd);
    close(fd);
}

void Server::use(std::string path, Handler handler){
    LOG_INFO << "Filter added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addFilter(path, handler);
}

void Server::get(std::string path, Handler handler){
    LOG_INFO << "GET handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(Method::GET, path, handler);
}

void Server::post(std::string path, Handler handler){
    LOG_INFO << "POST handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(Method::POST, path, handler);
}

void Server::put(std::string path, Handler handler){
    LOG_INFO << "PUT handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(Method::PUT, path, handler);
}

void Server::patch(std::string path, Handler handler){
    LOG_INFO << "PATCH handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(Method::PATCH, path, handler);
}

void Server::del(std::string path, Handler handler){
    LOG_INFO << "DELETE handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(Method::DELETE, path, handler);
}

void Server::request(Method method, std::string path, Handler handler){
    LOG_INFO << "Handler added on path " << path;
    assertServerState(ServerState::STARTING);
    radixTree.addHandler(method, path, handler);
}

void Server::assertServerState(ServerState desiredState){
    if (state != desiredState){
        throw std::runtime_error("assert server state failed");
    }
}

void Server::assertServerStateNot(ServerState nonDesiredState){
    if (state == nonDesiredState){
        throw std::runtime_error("assert server state failed");
    }
}