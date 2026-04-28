#pragma once
#include <string>
#include <unordered_map>

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
    CONNECT
};

Method getMethod(std::string m);
bool methodHasBody(Method m);


// STATUS CODE

const int StatusOK = 200;
const int StatusCreated = 201;
const int StatusNoContent = 204;

const int StatusBadRequest = 400;
const int StatusUnauthorized = 401;
const int StatusForbidden = 403;
const int StatusNotFound = 404;
const int StatusConflict = 409;

const int StatusInternalServerError = 500;
const int StatusNotImplemented = 501;

extern const std::unordered_map<int, std::string> statusMessages;

} // network 

} // server 
