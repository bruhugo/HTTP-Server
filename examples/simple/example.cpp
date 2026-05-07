#include "Server.hpp"
#include "Logger.hpp"
#include "Cookie.hpp"

#include <optional>


using namespace server::network;
using namespace server::basic;

int main(){
    Logger::setLogLevel("DEBUG");

    Server server(20);

    server.use("/", [](Context& ctx){
        LOG_DEBUG << "Log before chain";
        ctx.next();
        LOG_DEBUG << "Log after chain";
    });

    server.get("/health", [](Context& ctx){
        ctx.res.JSON(200, "{\"status\": \"active\"}");
    });

    server.get("/echo/:echomsg", [](Context& ctx){
        auto msg = ctx.params.at("echomsg");
        ctx.res.setBody(msg);
    });

    server.post("/echo", [](Context& ctx){
        auto body = ctx.req.body;
        ctx.res.setBody(body);
    });

    server.get("/cookie", [](Context& ctx){
        std::optional<Cookie> cookieopt = ctx.req.cookies.get("cookie");
        if (!cookieopt){
            LOG_DEBUG << "cookie not found";
        }else {
            LOG_DEBUG << "cookie found with value" << cookieopt.value().value;
        }

        CookieBuilder builder;
        Cookie cookie = builder
            .name("cookie")
            .value("hello world")
            .build();

        ctx.res.cookies.add(cookie);
    });

    server.listenPort("8080");
    return 0;
}