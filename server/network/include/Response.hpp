#pragma once

#include "Header.hpp"
#include "Cookie.hpp"

#include <string>


namespace server {
namespace network{

class Response {
public:
    Response();
    Response(int status, std::string body);

    Headers headers;

    void setStatusCode(int code) noexcept;
    void setBody(std::string) noexcept;

    void JSON(int status, std::string body);
    void text(int status, std::string body);
    void javaScript(int status, std::string body);
    void CSS(int status, std::string body);
    void HTML(int status, std::string body);

    std::string encode();

    Cookies cookies;
private:
    int statusCode; 
    std::string body; 
};

} // network
} // server
