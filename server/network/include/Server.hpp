#include <cstdint>
#include <vector>
#include "../../basic/include/ThreadPool.hpp"
#include "Connection.hpp"

using namespace std;

namespace server {

namespace network {

class Server {
public:
    Server(uint32_t threads);
    ~Server();

    void start();
private: 
    basic::ThreadPool tp;
    vector<Connection> connectins;
};

} // network

} // server
