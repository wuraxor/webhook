#include <iostream>
#include <nlohmann/json.hpp>
#include <webhook/config.h>
#include <webhook/events.h>
#include <webhook/server.h>

int main() {
    Config::load();
    Server server;
    server.start();
    return 0;
}