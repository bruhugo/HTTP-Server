#include "Response.hpp"
#include "Types.hpp"

#include <sstream>

using namespace server::network;

Response::Response(): statusCode{Status::OK}{}
Response::Response(int status, std::string msg): 
    statusCode{status}, body{std::move(msg)}{}

void Response::setStatusCode(int status) noexcept{
    statusCode = status;
}

void Response::setBody(std::string otherbody) noexcept {
    body = std::move(otherbody);
}

std::string Response::encode(){
    std::stringstream stream;
    std::string br = "\r\n";

    // Ensure Content-Length is set if body is not empty
    if (!body.empty() && headers.get(HeaderTypes::ContentLength) == std::nullopt) {
        headers.set(HeaderTypes::ContentLength, std::to_string(body.size()));
    }

    std::string reason = "Response";
    auto it = statusMessages.find(statusCode);
    if (it != statusMessages.end())
        reason = it->second;

    // status line
    stream 
        << "HTTP/1.1 " 
        << statusCode 
        << " " 
        << reason
        << br;
    
    // headers
    auto h = headers.data();

    for (auto const& [key, values] : h)
        for (auto const& value : values)
            stream << key << ": " << value << br;
    
    // body
    stream << br << body;

    return stream.str();
}