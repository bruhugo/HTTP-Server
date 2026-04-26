#pragma once

#include <functional>

namespace server
{
namespace network 
{

enum class ConnectionState {
    OPEN, CLOSED, CLOSING
};

using CloseFunc = std::function<void(int fd)>;

class Connection {
public:
    Connection(int fd, CloseFunc func);
    ~Connection();

    void close();
private:
    int fd;
    int epollfd;
    CloseFunc closeFunc;
};

} // basic
    
} // server
