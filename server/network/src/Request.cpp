#include "Request.hpp"
#include "Types.hpp"
#include <stdexcept>

using namespace server::network;

// --- REQUEST --- 
Request::Request(shared_ptr<Connection> connPtr): 
connPtr{connPtr}{}



// --- REQUEST PARSER --- 
RequestParser::RequestParser(shared_ptr<Connection> c):
state{REQUEST_LINE}, request(c){}

Request RequestParser::getRequest() {
    if (state != REQUEST_DONE) 
        throw runtime_error("request is not done parsing");
    return request;
}

bool RequestParser::parse(string& buffer){
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
}

// REQUEST LINE

static Method parseMethod (string& method) {
    // TODO: validade method
    return getMethod(method);
}

static Path parsePath(string& token) {
    // TODO: parse path fr
    Path path;
    path.URL = token;
    return;
}

static string parseHttpVersion(string& token) {
    // TODO: validade and only accept HTTP/1.1
    return token;
}

bool RequestParser::parseRequestLine(string& buffer){
    if (parseLine(buffer, requestLine)){
        vector<string> tokens = parseTokens(requestLine);
        if (tokens.size() != 3)
            throw runtime_error("bad request provided.");

        Method method = parseMethod(tokens.at(0));
        canHaveBody = methodHasBody(method);

        request.path = parsePath(tokens.at(1));
        request.httpVersion = (tokens.at(2));
    }else {
        return false;
    }
}


// REQUEST HEADERS

bool parseHeaders(string& buffer){ 

}

bool parseBody( string& buffer){

}

bool RequestParser::parseLine(string& buffer, string& line) {
    char prev = '\0';
    if (!line.empty()) prev = line.back();

    for (int i = offset; i < buffer.size(); i++){
        char cur = buffer.at(i);
        if (prev == '\r' && cur == '\n') {
            line.append(buffer, i);
            offset = i + 1;
            return true;
        }
        prev = cur;
    }

    line.append(buffer);
    return false;
}

vector<string> RequestParser::parseTokens(string& line){
    vector<string> tokens;
    string token;

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
