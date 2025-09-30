#include <iostream>
#include <nlohmann/json.hpp>
#include <webhook/events.h>
#include <webhook/server.h>

int main() {
    Server server;
    server.start();
    return 0;
}