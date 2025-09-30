#include <iostream>
#include <regex>
#include <webhook/discord.h>
#include <webhook/server.h>

static const auto re = std::regex("/([^/]+)/([^/]+)");

static auto githubEvent = "X-GitHub-Event";
static auto githubSig = "X-Hub-Signature-256";
static auto githubSigPrefix = "sha256=";
static auto githubUserAgentPrefix = "GitHub-Hookshot/";

Server::Server() : server(8080) {
    auto res = server.listen();
    if (!res.first) {
    std::cerr << res.second << std::endl;
    exit(1);
    }

    server.setOnConnectionCallback(
    [](const ix::HttpRequestPtr& request,
    const std::shared_ptr<ix::ConnectionState>& connectionState) -> ix::HttpResponsePtr {
        if (request->uri == "/health") {
            if (request->method != "GET") {
                return std::make_shared<ix::HttpResponse>(405, "Method " + request->method + " Not Allowed",
                    ix::HttpErrorCode::Ok,
                    ix::WebSocketHttpHeaders{{"allow", "GET"}},
                "");
            }
            return std::make_shared<ix::HttpResponse>(200, "OK",
               ix::HttpErrorCode::Ok,
                ix::WebSocketHttpHeaders(),
            R"({"status": "ok"})");
        }
        if (std::regex_match(request->uri, re)) {
            if (request->method != "POST") {
                return std::make_shared<ix::HttpResponse>(405, "Method " + request->method + " Not Allowed",
                    ix::HttpErrorCode::Ok,
                    ix::WebSocketHttpHeaders{{"allow", "POST"}},
                "");
            }
            std::smatch m;
            std::regex_match(request->uri, m, re);
            auto id = m[1].str();
            auto token = m[2].str();
            if (request->headers.at("content-type") != "application/json") {
                return std::make_shared<ix::HttpResponse>(415,
                    "Content-Type " + request->headers.at("content-type") + " Not Allowed",
                    ix::HttpErrorCode::Ok,
                    ix::WebSocketHttpHeaders{{"Accept-Post", "application/json"}},
                "");
            }
            auto event = request->headers.at(githubEvent);
            if (event.empty()) {
                return std::make_shared<ix::HttpResponse>(400, "Event can not be empty",
                    ix::HttpErrorCode::Ok,
                    ix::WebSocketHttpHeaders{},
                "");
            }

            if (!request->headers.at("User-Agent").starts_with(githubUserAgentPrefix)) {
                return std::make_shared<ix::HttpResponse>(400, "Invalid user agent",
                    ix::HttpErrorCode::Ok,
                    ix::WebSocketHttpHeaders{},
                "");
            }

            Discord::Send(event, request->body, {id, token});

            return std::make_shared<ix::HttpResponse>(200, "OK",
                ix::HttpErrorCode::Ok,
                ix::WebSocketHttpHeaders(),
            "");
        }
        return std::make_shared<ix::HttpResponse>(404, request->uri + " Not Found",
            ix::HttpErrorCode::Ok,
            ix::WebSocketHttpHeaders{},
        "");
    });

}

void Server::start() {
    server.start();
    server.wait();
}

/*

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
    Discord::Send(event, std::string{req->getBody()}, {id, token});

    callback(resp);
}
 */