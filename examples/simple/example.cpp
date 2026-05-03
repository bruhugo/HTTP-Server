#include "Server.hpp"

using namespace server::network;

int main(){
    Server server(20);

    server.get("/hello", [](Context& ctx){
        ctx.res.setBody("hello my friend");
    });

    server.get("/echo/:echomsg", [](Context& ctx){
        auto msg = ctx.params.at("echomsg");
        ctx.res.setBody(msg);
    });

    server.post("/echo", [](Context& ctx){
        auto body = ctx.req.body;
        ctx.res.setBody(body);
    });

    server.listenPort("8080");
    return 0;
}