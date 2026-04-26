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

Method getMethod(std::string m){
    if (m == "OPTIONS")     return Method::OPTIONS;
    if (m == "GET")         return Method::GET;
    if (m == "HEAD")        return Method::HEAD;
    if (m == "POST")        return Method::POST;
    if (m == "PUT")         return Method::PUT;
    if (m == "PATCH")       return Method::PATCH;
    if (m == "DELETE")      return Method::DELETE;
    if (m == "TRACE")       return Method::TRACE;
    if (m == "CONNECT")     return Method::CONNECT;
}

bool methodHasBody(Method m) {
    switch (m) {
        case Method::PUT:
            return true;
        case Method::POST:
            return true;
        case Method::PATCH:
            return true;
        default:
            return false;
    }
}
    
} // network 

} // server 