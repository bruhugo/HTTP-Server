#include "Request.hpp"
#include "Types.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <string_view>

using namespace server::network;

// --- REQUEST --- 
Request::Request(int conn): connsocket{conn}{}

// --- REQUEST PARSER --- 
RequestParser::RequestParser(int connfd): state(REQUEST_LINE), offset(0), canHaveBody(false), contentLength(0), fd{connfd}, request(fd){}

Request RequestParser::getRequest() {
    if (state != REQUEST_DONE) 
        throw std::runtime_error("request is not done parsing");
    return request;
}

static std::vector<std::string> parseTokens(std::string_view line);
static Method parseMethod (std::string_view method);
static Path parsePath(std::string_view token);
static std::string parseHttpVersion(std::string_view token);

struct Header {
    std::string key;
    std::string value;
};
static Header parseHeaderLine(std::string_view line);

bool RequestParser::parse(std::string_view buffer){
    offset = 0;
    
    while (offset < buffer.size()) {
        if (state == REQUEST_LINE) {
            if (parseRequestLine(buffer)) {
                state = REQUEST_HEADERS;
            } else {
                return false;
            }
        }

        if (state == REQUEST_HEADERS) {
            while (offset < buffer.size()) {
                if (parseHeaders(buffer)) {
                    if (canHaveBody && contentLength > 0) {
                        state = REQUEST_BODY;
                    } else {
                        state = REQUEST_DONE;
                        return true;
                    }
                    break;
                }
            }
        }

        if (state == REQUEST_BODY) {
            if (parseBody(buffer.substr(offset))) {
                state = REQUEST_DONE;
                return true;
            }
            return false;
        }

        if (state == REQUEST_DONE) return true;
    }
    
    return state == REQUEST_DONE;
}

bool RequestParser::parseRequestLine(std::string_view buffer){
    if (parseLine(buffer, requestLine)){
        std::vector<std::string> tokens = parseTokens(requestLine);
        if (tokens.size() != 3)
            throw std::runtime_error("bad request provided.");

        request.method = parseMethod(tokens.at(0));
        request.path = parsePath(tokens.at(1));
        request.httpVersion = parseHttpVersion(tokens.at(2));

        canHaveBody = methodHasBody(request.method);

        return true;
    }
    return false;
}

bool RequestParser::parseHeaders(std::string_view buffer){ 
    if (parseLine(buffer, currentHeader)){
        if (currentHeader.empty())
            return true;
        
        Header header = parseHeaderLine(currentHeader);
        request.headers.set(header.key, header.value);

        if (header.key == HeaderTypes::ContentLength)
            contentLength = std::stoul(header.value);

        currentHeader.clear();
        return false;
    }
    return false;
}

bool RequestParser::parseBody(std::string_view buffer){
    request.body.append(buffer);
    offset += buffer.size();
    return request.body.size() >= contentLength;
}

bool RequestParser::parseLine(std::string_view buffer, std::string& line) {
    for (size_t i = offset; i < buffer.size(); ++i){
        char cur = buffer.at(i);
        if (cur == '\n' && !line.empty() && line.back() == '\r') {
            line.pop_back(); // remove the \r
            offset = i + 1;
            return true;
        }
        line += cur;
    }
    return false;
}

static std::vector<std::string> parseTokens(std::string_view line){
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = line.find(' ');

    while (end != std::string_view::npos) {
        if (end > start) {
            tokens.emplace_back(line.substr(start, end - start));
        }
        start = end + 1;
        end = line.find(' ', start);
    }
    if (start < line.size()) {
        tokens.emplace_back(line.substr(start));
    }

    return tokens;
}

static Method parseMethod (std::string_view method) {
    return getMethod(std::string(method));
}

static Path parsePath(std::string_view token) {
    Path path;
    path.URL = std::string(token);
    return path;
}

static std::string parseHttpVersion(std::string_view token) {
    return std::string(token);
}

static Header parseHeaderLine(std::string_view line){
    auto pos = line.find(':');
    if (pos == std::string_view::npos) 
        throw std::runtime_error("invalid header"); 

    std::string key = std::string(line.substr(0, pos));
    std::string value = std::string(line.substr(pos + 1));
    
    // Trim value
    auto first = value.find_first_not_of(" ");
    if (std::string::npos != first) {
        auto last = value.find_last_not_of(" ");
        value = value.substr(first, (last - first + 1));
    }

    return {key, value};
}
