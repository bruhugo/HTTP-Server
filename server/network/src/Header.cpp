#include "Header.hpp"

using namespace server::network;

optional<HeaderList> Headers::get(string headerName) {
    try {
        auto value = headers.at(headerName);
        return make_optional<HeaderList>(value);
    }catch (out_of_range &err) {
        return nullopt;
    }
}

optional<string> Headers::getFirst(string headerName) {
    try {
        auto value = headers.at(headerName);
        return make_optional<string>(value.front());
    }catch (out_of_range &err) {
        return nullopt;
    }
}

void Headers::set(string key, string value) {
    auto it = headers.find(key);
    if (it == headers.end()) {
        headers.emplace(key, vector<string, HeaderList>({value}));
    }else {
        (*it).second.push_back(value);
    }
}