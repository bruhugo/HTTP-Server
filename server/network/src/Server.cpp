#include "Server.hpp"
#include "Request.hpp"

#include <algorithm>

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
    sockaddr addr;
    epoll_event ev, events[MAX_EVENTS];

    hint.ai_family = AF_UNSPEC;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(nullptr, port.data(), &hint, &res) != 0) 
        throw std::runtime_error(strerror(errno));

    freeaddrinfo(&hint);
    for (p = res; p != nullptr; p = p->ai_next){
        serverfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (serverfd == 0) {
            perror("creating socket");
            continue;
        }

        int err = bind(serverfd, p->ai_addr, p->ai_addrlen);
        if (err != 0) {
            perror("binding address");
            continue;
        }

        break;
    }

    if (serverfd == 0)
        throw std::runtime_error("error opening socket");

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
        int eventn = epoll_wait(epollfd, events, MAX_EVENTS, 0);
        if (eventn == -1) 
            // TODO: maybe change to resilient error handling
            throw std::runtime_error(strerror(errno));

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
        throw std::system_error(errno,
        std::generic_category(), 
        "error accepting request");
    }

    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1){
        perror("error adding fd to epoll");
        return;
    }

    std::lock_guard<std::mutex> lk(mu);
    connections.emplace(fd, Connection(fd));
}

void Server::handleRequest(int fd){ 
    tp.submit([this, fd]{
        std::lock_guard<std::mutex> lk(mu);
        Connection conn = connections[fd];

        std::optional<Request> request = conn.parseRequest();
        if (!request.has_value())
            // request is not done yet
            return;

        // handle request later

        // close connection if request demands it
    });
}

void Server::closeConnection(int fd){
    std::lock_guard<std::mutex> lk(mu);
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr) == -1){
        perror("failed to remove connection from epoll");
    }
    connections.erase(fd);
    close(fd);
}