#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace server
{

namespace network
{

// METHOD
enum class Method {
    OPTIONS, 
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DELETE,
    TRACE,
    CONNECT,

    // dont use this
    NUMBER
};


Method getMethod(std::string m);
bool methodHasBody(Method m);


// STATUS CODE

namespace Status{
    const int OK = 200;
    const int Created = 201;
    const int NoContent = 204;

    const int BadRequest = 400;
    const int Unauthorized = 401;
    const int Forbidden = 403;
    const int NotFound = 404;
    const int Conflict = 409;

    const int InternalServerError = 500;
    const int NotImplemented = 501;
}


extern const std::unordered_map<int, std::string> statusMessages;

} // network 

} // server 
