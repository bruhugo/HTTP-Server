#pragma once
#include <string>

namespace server
{

namespace network
{

enum class Method {
    OPTIONS, 
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DELETE,
    TRACE,
    CONNECT
};

Method getMethod(std::string m);
bool methodHasBody(Method m);
    
} // network 

} // server 
