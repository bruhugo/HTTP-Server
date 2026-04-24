#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

using namespace std;

namespace server
{

namespace network
{

using HeaderList = vector<string>;

namespace HeaderTypes {
const string ContentLength = "Content-Length";
const string Accept = "Accept";
const string AcceptCharset = "Accept-Charset";
}

struct Headers {

public:
    optional<HeaderList> get(string);
    optional<string> getFirst(string);

    // Overrides all previous set values of the given header
    void set(string, string);

    // Adds to the header list
    void add(string, string);
private:
    unordered_map<string, HeaderList> headers; 
};

} // namespace network

} // namespace server

