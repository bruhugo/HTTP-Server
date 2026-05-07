#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <optional>

namespace cr = std::chrono;

namespace server {
namespace network {

struct Cookie {
    static Cookie unmarshal(std::string str);
    std::string marshal();

    bool httpOnly;
    bool secure;
    std::string path;
    std::string value;
    std::string name;
    std::string domain;
    cr::system_clock::time_point expireAt;
};

class CookieBuilder {
public:
    CookieBuilder& name(std::string);
    CookieBuilder& value(std::string);
    CookieBuilder& path(std::string);
    CookieBuilder& secure(bool);
    CookieBuilder& HttpOnly(bool);
    CookieBuilder& domain(std::string domain);
    Cookie build();

private:
    Cookie cookie;
};

struct Cookies {
    void add(Cookie cookie);
    std::optional<Cookie> get(std::string name);
    std::unordered_map<std::string, Cookie> cookies;   
};

}
}


