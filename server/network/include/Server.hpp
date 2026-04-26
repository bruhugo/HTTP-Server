#include <cstdint>
#include <vector>
#include "../../basic/include/ThreadPool.hpp"
#include "Connection.hpp"
#include "mutex"

namespace server {

namespace network {

class Server {
public:
    Server(uint32_t threads);
    ~Server();

    void listenPort(std::string port);
private: 
    void acceptConnection(int fd);
    void handleRequest(int fd);
    void closeConnection(int conn);

    std::vector<int> connections;
    basic::ThreadPool tp;

    std::mutex mu;
    int epollfd;
    int serverfd;
};

} // network

} // server
