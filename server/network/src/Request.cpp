#include "Request.hpp"
#include "Types.hpp"
#include <stdexcept>
#include <sys/socket.h>

using namespace server::network;

// --- REQUEST --- 
Request::Request(int conn): 
connsocket{conn}{}

// --- REQUEST PARSER --- 
RequestParser::RequestParser(int connfd): fd{connfd}, request(fd){}

Request RequestParser::getRequest() {
    if (state == REQUEST_ERROR) 
        throw std::runtime_error("an error occured while parsing");

    if (state != REQUEST_DONE) 
        // TODO: replace by HttpError later
        throw std::runtime_error("request is not done parsing");
    return request;
}


std::vector<std::string> parseTokens(std::string_view line);
std::string trim(std::string_view parse);


static Method parseMethod (std::string& method);
static Path parsePath(std::string& token);
static std::string parseHttpVersion(std::string& token);

struct Header {
    std::string key;
    std::string value;
};
static Header parseHeaderLine(std::string& line);

bool RequestParser::parse(){
    char rawbuff[BUFFER_SIZE];
    int read = recv(fd, rawbuff, BUFFER_SIZE, 0);
    if (read == -1 || read == 0){
        state = REQUEST_ERROR;
        return true;
    }

    std::string_view buffer(rawbuff);

    offset = 0;
    if (state == REQUEST_LINE && parseRequestLine(buffer)){
        state = REQUEST_HEADERS;
    }else {
        return false;
    }

    if (state == REQUEST_HEADERS && parseHeaders(buffer)) {
        if (canHaveBody && contentLength > 0){
            state = REQUEST_DONE;
        }else {
            state = REQUEST_DONE;
            return true;
        }
    }else {
        return false;
    }

    if (state == REQUEST_BODY) {
    }
    return false;
}

// REQUEST LINE
bool RequestParser::parseRequestLine(std::string_view buffer){
    if (parseLine(buffer, requestLine)){
        std::vector<std::string> tokens = parseTokens(requestLine);
        if (tokens.size() != 3)
            throw std::runtime_error("bad request provided.");

        Method method = parseMethod(tokens.at(0));
        canHaveBody = methodHasBody(method);

        request.path = parsePath(tokens.at(1));
        request.httpVersion = (tokens.at(2));
    }else {
        return false;
    }
}


// REQUEST HEADERS
bool RequestParser::parseHeaders(std::string_view buffer){ 
    if (parseLine(buffer, currentHeader)){
        if (currentHeader.empty())
            return true;
        Header header = parseHeaderLine(currentHeader);
        request.headers.add(header.key, header.value);

        if (header.key == HeaderTypes::ContentLength)
            contentLength = std::stoi(header.value);

        currentHeader.clear();
        return false;
    }else {
        return false;
    }
}

bool RequestParser::parseBody(std::string_view buffer){
    // TODO: maybe support chunked content in an unlikely future
    body.append(buffer);
    return body.size() >= contentLength;
}

bool RequestParser::parseLine(std::string_view buffer, std::string& line) {
    char prev = '\0';
    if (!line.empty()) prev = line.back();

    for (int i = offset; i < static_cast<int>(buffer.size()); i++){
        char cur = buffer.at(i);
        if (prev == '\r' && cur == '\n') {
            line.pop_back();
            offset = i + 1;
            return true;
        }

        line += cur;
        prev = cur;
    }

    return false;
}

std::vector<std::string> parseTokens(std::string& line){
    std::vector<std::string> tokens;
    std::string token;

    for (char c : line) {
        if (c == ' ') {
            if (!token.empty()){
                tokens.push_back(token);
                token.clear();
            } 
            continue;
        }
        if (token.compare("\r\n"))
            return tokens;
        token += c;
    }

    return tokens;
}

static Method parseMethod (std::string& method) {
    // TODO: validade method
    return getMethod(method);
}

static Path parsePath(std::string& token) {
    // TODO: parse path fr
    Path path;
    path.URL = token;
    return path;
}

static std::string parseHttpVersion(std::string& token) {
    // TODO: validade and only accept HTTP/1.1
    return token;
}

static Header parseHeaderLine(std::string& line){
    auto pos = line.find(':');
    if (pos == std::string::npos) 
        throw std::runtime_error("invalid header"); 

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    return {key, value};
}