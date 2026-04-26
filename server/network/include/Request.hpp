#include <Connection.hpp>
#include <Header.hpp>
#include <Types.hpp>

#include <cstdint>
#include <memory>

#define BUFFER_SIZE 5000

namespace server {

namespace network {

struct Path {
    std::string URL;
    std::unordered_map<std::string, std::vector<std::string>> queries; 
};

class Request
{
public:
    Request(int conn);

    Path path;
    Headers headers;
    Method method;
    std::string body;
    std::string httpVersion;
    int connsocket;
};


class RequestParser {
public:
    RequestParser(int fd);
    // Gets the request
    // Throws if it was not done parsing
    Request getRequest();

    // Returns true if it's done parsing
    bool parse();
private:
    enum State {
        REQUEST_LINE,
        REQUEST_HEADERS, 
        REQUEST_BODY, 
        REQUEST_DONE,
        REQUEST_ERROR
    };

    bool parseRequestLine(std::string_view buffer);
    bool parseHeaders(std::string_view buffer);
    bool parseBody(std::string_view buffer);

    bool parseLine(std::string_view buffer, std::string& line);

    State state;

    std::string requestLine;
    std::string currentHeader;
    std::string body;

    Request request;
    size_t offset;

    bool canHaveBody;
    size_t contentLength;

    int fd;
};

} // network

} // server
