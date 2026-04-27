#include "Header.hpp"

using namespace server::network;

std::optional<HeaderList> Headers::get(std::string headerName) {
    try {
        auto value = headers.at(headerName);
        return std::make_optional<HeaderList>(value);
    }catch (std::out_of_range &err) {
        return std::nullopt;
    }
}

std::optional<std::string> Headers::getFirst(std::string headerName) {
    try {
        auto value = headers.at(headerName);
        return std::make_optional<std::string>(value.front());
    }catch (std::out_of_range &err) {
        return std::nullopt;
    }
}

void Headers::set(std::string key, std::string value) {
    auto it = headers.find(key);
    if (it == headers.end()) {
        headers.emplace(key, HeaderList({value}));
    }else {
        (*it).second.push_back(value);
    }
}
