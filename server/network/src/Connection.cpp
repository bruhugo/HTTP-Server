#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <system_error>
#include <optional>

#include <sys/socket.h>


#define BUFFER_SIZE 8192

using namespace server::network;

Connection::Connection(int socket): fd{socket}, parser(std::make_unique<RequestParser>(fd)){};
Connection::~Connection() = default;

Connection::Connection(Connection&&) noexcept = default;
Connection& Connection::operator=(Connection&&) noexcept = default;

std::optional<Request> Connection::parseRequest(){
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t read = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT); 

        if (read == -1){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }else {
                throw std::system_error(errno, 
                    std::generic_category(), 
                    "error receiving data");
            }
        }else if (read == 0){
            // Use a specific error message or throw to signal connection close
            throw std::runtime_error("connection closed");
        }else {
            if (parser->parse(std::string_view(buffer, read)))
                return std::make_optional(parser->getRequest());
        }
    }

    return std::nullopt; 
}

void Connection::writeResponse(const std::string& str){
    size_t total_sent = 0;

    while (total_sent < str.size()) {
        ssize_t sent = send(fd, str.c_str() + total_sent, str.size() - total_sent, MSG_NOSIGNAL);
        
        if (sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // In a production server, we would re-arm epoll for EPOLLOUT.
                // For now, we'll block or return. Given the current architecture,
                // we'll treat it as a failure to keep it simple but safe.
                throw std::system_error(errno, std::generic_category(), "send would block");
            } else {
                throw std::system_error(errno, std::generic_category(), "error sending data");
            }
        }
        total_sent += sent;
    }
}