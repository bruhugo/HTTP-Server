#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

namespace server
{

namespace network
{

using HeaderList = std::vector<std::string>;

namespace HeaderTypes {
const std::string ContentLength = "Content-Length";
const std::string Accept = "Accept";
const std::string AcceptCharset = "Accept-Charset";
}

struct Headers {

public:
    std::optional<HeaderList> get(std::string);
    std::optional<std::string> getFirst(std::string);

    // Overrides all previous set values of the given header
    void set(std::string, std::string);

    // Adds to the header list
    void add(std::string, std::string);
private:
    std::unordered_map<std::string, HeaderList> headers; 
};

} // namespace network

} // namespace server

