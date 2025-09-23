#include <webhook/controller.h>

void Controller::health(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
}

void Controller::webhook(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string&&, std::string&&) {
}
