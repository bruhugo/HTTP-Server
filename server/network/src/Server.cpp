#include "Server.hpp"
#include "Request.hpp"

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

#define MAX_EVENTS 30
#define REQUEST_BUFFER_SIZE 5000

using namespace server::network;

Server::Server(uint32_t threads): tp(threads) {}
Server::~Server() {}

void Server::listenPort(std::string port){

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

        break;
    }

    freeaddrinfo(res);

    if (p == nullptr)
        throw std::runtime_error("error opening socket or binding");

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

    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
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
    tp.submit([this, fd]{
        std::lock_guard<std::mutex> lk(mu);
        auto it = connections.find(fd);
        if (it == connections.end()) return;

        Connection& conn = it->second;

        try {
            std::optional<Request> request = conn.parseRequest();
            if (!request.has_value())
                return;
            
            // TODO: logic to handle request
        } catch (const std::exception& e) {
            // handle error or close connection
        }
    });
}

void Server::closeConnection(int fd){
    std::lock_guard<std::mutex> lk(mu);
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr) == -1){
        // ignore error if already removed
    }
    connections.erase(fd);
    close(fd);
}
