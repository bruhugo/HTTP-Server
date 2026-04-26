#pragma once

#include "Request.hpp"

#include <functional>
#include <optional>

namespace server
{
namespace network 
{

class Connection {
public:
    Connection(int fd);
    std::optional<Request> parseRequest();
    void writeResponse();
private:
    RequestParser parser;
    int fd;
};

} // basic
    
} // server
