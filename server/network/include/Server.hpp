#pragma once

#include "Connection.hpp"
#include "ThreadPool.hpp"
#include "Context.hpp"
#include "RadixTree.hpp"

#include <cstdint>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <string>

namespace server {
namespace network {

enum class ServerState{
    STARTING, LISTENING
};

class Server {
public:
    Server(uint32_t threads);
    ~Server();

    void use(std::string path, Handler handler);

    void get(std::string path, Handler handler);
    void post(std::string path, Handler handler);
    void put(std::string path, Handler handler);
    void patch(std::string path, Handler handler);
    void del(std::string path, Handler handler);

    void request(Method method, std::string path, Handler handler);
    
    void listenPort(std::string port);
private: 
    void acceptConnection();
    void handleRequest(int fd);
    void closeConnection(int conn);

    void assertServerState(ServerState state);
    void assertServerStateNot(ServerState state);

    std::unordered_map<int, Connection> connections;
    basic::ThreadPool tp;
    std::mutex mu;
    ServerState state;
    RadixTree radixTree;

    int epollfd;
    int serverfd;
};

} // network

} // server
