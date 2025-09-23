#pragma once

#include <drogon/HttpController.h>

class Controller : public drogon::HttpController<Controller> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Controller::health, "/health", drogon::Post);
    ADD_METHOD_TO(Controller::webhook, "/{id}/{token}", drogon::Post);
    METHOD_LIST_END
protected:
    void health(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void webhook(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        std::string&&, std::string&&);
};