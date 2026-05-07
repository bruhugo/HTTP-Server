#include "Cookie.hpp"
#include "Error.hpp"

#include <vector>
#include <iomanip>
#include <sstream>
#include <ctime>

using namespace server::network;

static std::vector<std::string> breakCookieHeader(std::string str){
    std::vector<std::string> strings;

    size_t start = 0;
    size_t end = str.find(";");

    while (end != std::string::npos){
        if (end > start){
           strings.emplace_back(str.substr(start, end - start));
        }
        start = end + 1;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t')) start++;
        end = str.find(";", start);
    }

    if (start < str.size()) 
        strings.emplace_back(str.substr(start));

    return strings;
}

Cookie Cookie::unmarshal(std::string str){
    Cookie cookie{};
    auto attributes = breakCookieHeader(str);

    if (attributes.empty()) return cookie;

    size_t pos = attributes[0].find('=');
    if (pos != std::string::npos) {
        cookie.name = attributes[0].substr(0, pos);
        cookie.value = attributes[0].substr(pos + 1);
    }

    for (size_t i = 1; i < attributes.size(); ++i) {
        std::string attr = attributes[i];
        if (attr == "Secure") {
            cookie.secure = true;
        } else if (attr == "HttpOnly") {
            cookie.httpOnly = true;
        } else {
            size_t eqPos = attr.find('=');
            if (eqPos != std::string::npos) {
                std::string key = attr.substr(0, eqPos);
                std::string val = attr.substr(eqPos + 1);
                if (key == "Path") cookie.path = val;
                else if (key == "Domain") cookie.domain = val;
                else if (key == "Expires") {
                    std::tm tm = {};
                    std::istringstream ss(val);
                    ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
                    if (!ss.fail()) {
                        cookie.expireAt = cr::system_clock::from_time_t(timegm(&tm));
                    }
                }
            }
        }
    }
    return cookie;
}

std::string Cookie::marshal(){
    std::string str = name + "=" + value;
    if (!domain.empty()) str += "; Domain=" + domain;
    if (!path.empty()) str += "; Path=" + path;
    
    if (expireAt != cr::system_clock::time_point{}) {
        std::time_t tt = cr::system_clock::to_time_t(expireAt);
        std::tm* gmt = std::gmtime(&tt);
        if (gmt) {
            std::stringstream ss;
            ss << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
            str += "; Expires=" + ss.str();
        }
    }

    if (secure) str += "; Secure";
    if (httpOnly) str += "; HttpOnly";

    return str;
}

CookieBuilder& CookieBuilder::name(std::string name){
    cookie.name = name;
    return *this;
}

CookieBuilder& CookieBuilder::value(std::string value){
    cookie.value = value;
    return *this;
}

CookieBuilder& CookieBuilder::path(std::string path){
    cookie.path = path;
    return *this;
}

CookieBuilder& CookieBuilder::secure(bool secure){
    cookie.secure = secure;
    return *this;
}

CookieBuilder& CookieBuilder::HttpOnly(bool httpOnly){
    cookie.httpOnly = httpOnly;
    return *this;
}

CookieBuilder& CookieBuilder::domain(std::string domain){
    cookie.domain = domain;
    return *this;
}

Cookie CookieBuilder::build(){
    return cookie;
}

void Cookies::add(Cookie cookie){
    if (cookie.name.empty()) 
        throw std::runtime_error("Cookie must have a name.");

    cookies.emplace(cookie.name, cookie);
}

std::optional<Cookie> Cookies::get(std::string name){
    auto it = cookies.find(name);
    if (it == cookies.end())
        return std::nullopt;

    return it->second;
}
