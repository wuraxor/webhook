#include <webhook/controller.h>
#include <webhook/discord.h>

static auto githubEvent = "X-GitHub-Event";
static auto githubSig = "X-Hub-Signature-256";
static auto githubSigPrefix = "sha256=";
static auto githubUserAgentPrefix = "GitHub-Hookshot/";

void Controller::health(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    Json::Value ret;
    ret["status"] = "ok";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void Controller::webhook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string&& id, std::string&& token) {
    if (req->contentType() != drogon::ContentType::CT_APPLICATION_JSON) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::HttpStatusCode::k415UnsupportedMediaType);
        resp->addHeader("Accept-Post", "application/json");
        callback(resp);
        return;
    }

    auto event = req->getHeader(githubEvent);
    if (event.empty()) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Missing event");
        callback(resp);
        return;
    }

    if (!req->getHeader("User-Agent").starts_with(githubUserAgentPrefix)) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid user agent");
        callback(resp);
        return;
    }

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k202Accepted);
    callback(resp);

    Discord(event, std::string{req->getBody()}, {id, token});
}
