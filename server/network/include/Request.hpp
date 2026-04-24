#include <Connection.hpp>
#include <Header.hpp>

#include <cstdint>
#include <memory>


using namespace std;

namespace server {

namespace network {



struct Path {
    string URL;
    unordered_map<string, vector<string>> queries; 
};

class Request
{
public:
    Request(shared_ptr<Connection> connPtr);

    shared_ptr<Connection> connPtr;

    Path path;
    Headers headers;
    Method method;
    string body;
    string httpVersion;
private:

};


class RequestParser {
public:
    RequestParser(shared_ptr<Connection> connPtr);
    // Gets the request
    // Throws if it was not done parsing
    Request getRequest();

    // Returns true if it's done parsing
    bool parse(string& buffer);
private:
    enum State {
        REQUEST_LINE,
        REQUEST_HEADERS, 
        REQUEST_BODY, 
        REQUEST_DONE
    };

    bool parseRequestLine(string_view buffer);
    bool parseHeaders(string_view buffer);
    bool parseBody( string_view buffer);

    bool parseLine(string_view buffer, string& line);

    State state;

    string requestLine;
    string currentHeader;
    string body;

    Request request;
    size_t offset;

    bool canHaveBody;
    size_t contentLength;
};

} // network

} // server
