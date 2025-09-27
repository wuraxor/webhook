#include <webhook/server.h>
#include <drogon/HttpAppFramework.h>

void Server::start() {
    drogon::app().addListener("0.0.0.0",8080);
    drogon::app().run();
}

