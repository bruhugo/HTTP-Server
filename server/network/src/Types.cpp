#include "Types.hpp"
#include <stdexcept>

namespace server
{
namespace network
{

const std::unordered_map<int, std::string> statusMessages = {
    {StatusOK, "OK"},
    {StatusCreated, "Created"},
    {StatusNoContent, "No Content"},
    {StatusBadRequest, "Bad Request"},
    {StatusUnauthorized, "Unauthorized"},
    {StatusForbidden, "Forbidden"},
    {StatusNotFound, "Not Found"},
    {StatusConflict, "Conflict"},
    {StatusInternalServerError, "Internal Server Error"},
    {StatusNotImplemented, "Not Implemented"}
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
    throw std::runtime_error("Invalid HTTP method: " + m);
}

bool methodHasBody(Method m) {
    switch (m) {
        case Method::PUT:
        case Method::POST:
        case Method::PATCH:
            return true;
        default:
            return false;
    }
}
    
} // network 
} // server 
