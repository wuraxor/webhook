#pragma once
#include <ixwebsocket/IXHttpServer.h>

class Server {
public:
    Server();
    void start();
private:
    ix::HttpServer server;
};
