#include "Server.hpp"

int main(){
    server::network::Server server(20);
    server.listenPort("8080");
    return 0;
}