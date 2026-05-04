#include "Server.hpp"
#include "Logger.hpp"

using namespace server::network;
using namespace server::basic;

int main(){
    Logger::setLogLevel("DEBUG");

    Server server(20);

    server.serveHttpFile("/index.html", "static/test.html");

    server.listenPort("8080");
    return 0;
}
