#pragma once

#include "Header.hpp"

#include <string>


namespace server {
namespace network{

class Response {
public:
    Response();
    Response(int status, std::string body);

    Headers headers;

    void setStatusCode(int code) noexcept;

    void setBody(std::string);

    std::string encode();

private:
    int statusCode; 
    std::string body; 
};

} // network
} // server
