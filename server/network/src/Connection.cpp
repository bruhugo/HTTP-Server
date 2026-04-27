#include "Connection.hpp"
#include "Request.hpp"

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
        int read = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT); 

        if (read == -1){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }else {
                throw std::system_error(errno, 
                    std::generic_category(), 
                    "error receiving data");
            }
        }else if (read == 0){
            throw std::runtime_error("connection closed");
        }else {
            if (parser->parse(std::string_view(buffer, read)))
                return std::make_optional(parser->getRequest());
        }
    }

    return std::nullopt; 
}
