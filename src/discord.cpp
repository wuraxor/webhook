#include <utility>
#include <drogon/HttpClient.h>
#include <webhook/discord.h>
#include <webhook/events.h>

using EventFunc = std::optional<Webhook>(*)(const Json::Value&);

static const std::map<std::string, EventFunc> EventMap = {
    {"check_run", &CheckRunFunc},
    {"fork", &ForkFunc},
    {"issue_comment", &IssueCommentFunc},
    {"issues", &IssuesFunc},
    {"public", &PublicFunc},
    {"pull_request", &PullRequestFunc},
    {"push", &PushFunc},
    {"release", &ReleaseFunc},
    {"repository", &RepositoryFunc},
    {"star", &StarFunc},
    {"workflow_run", &WorkflowRunFunc},
};

Credentials::Credentials(std::string  id, std::string  secret) : _id(std::move(id)), _secret(std::move(secret)) {}

const std::string& Credentials::id() const {
    return _id;
}

const std::string& Credentials::secret() const {
    return _secret;
}

void Discord::Send(const std::string& event, const std::string& body, const Credentials& credentials) {
    auto it = EventMap.find(event);
    if (it == EventMap.end()) {
        return;
    }
    Json::Value bodyJson;
    Json::Reader().parse(body, bodyJson);
    auto webhook = it->second(bodyJson);

    if (webhook.has_value()) {
        Send(webhook.value(), credentials);
    }
}

void Discord::Send(const Webhook& w, const Credentials& credentials) {
    auto client = drogon::HttpClient::newHttpClient("https://discord.com");

    Json::Value bodyj;
    bodyj["content"] = w.content;
    bodyj["username"] = w.username;
    bodyj["avatar_url"] = w.avatarUrl;

    Json::FastWriter writer;
    std::string body = writer.write(bodyj);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setPath(std::format("/api/webhooks/{}/{}", credentials.id(), credentials.secret()));
    request->setContentTypeCode(drogon::ContentType::CT_APPLICATION_JSON);
    request->setBody(body);
    request->setMethod(drogon::Post);

    client->sendRequest(request, [](drogon::ReqResult result, const drogon::HttpResponsePtr& res) {
    });
}
