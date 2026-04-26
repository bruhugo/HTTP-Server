#include "Connection.hpp"
#include "../../basic/include/ThreadPool.hpp"

#include <cstdint>
#include <vector>
#include <mutex>
#include <unordered_map>

namespace server {

namespace network {

class Server {
public:
    Server(uint32_t threads);
    ~Server();

    void listenPort(std::string port);
private: 
    void acceptConnection();
    void handleRequest(int fd);
    void closeConnection(int conn);

    std::unordered_map<int, Connection> connections;
    basic::ThreadPool tp;

    std::mutex mu;
    int epollfd;
    int serverfd;
};

} // network

} // server
