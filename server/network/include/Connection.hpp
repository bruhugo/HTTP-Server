#pragma once

#include <functional>
#include <optional>
#include <memory>

namespace server
{
namespace network 
{

class Request;
class RequestParser;

class Connection {
public:
    Connection(int fd);
    ~Connection();

    Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&) noexcept;
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    std::optional<Request> parseRequest();
    void writeResponse(const std::string& str);
private:
    std::unique_ptr<RequestParser> parser;
    int fd;
};

} // network
    
} // server
