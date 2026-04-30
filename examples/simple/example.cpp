#include "Server.hpp"

using namespace server::network;

int main(){
    Server server(20);

    server.get("/echo", [](Context& ctx){
        std::string body = ctx.req.body;
        ctx.res.setBody(body);
    });

    server.listenPort("8080");
    return 0;
}