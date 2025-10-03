#include <iostream>
#include <utility>
#include <webhook/discord.h>
#include <webhook/events.h>
#include <ixwebsocket/IXHttpClient.h>
#include <webhook/config.h>

using EventFunc = std::optional<Webhook>(*)(const nlohmann::json&);

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
    nlohmann::json bodyJson = nlohmann::json::parse(body);
    auto webhook = it->second(bodyJson);

    if (webhook.has_value()) {
        Send(webhook.value(), credentials);
    }
}

void Discord::Send(const Webhook& w, const Credentials& credentials) {
    nlohmann::json bodyj = {{"content", w.content}, {"username", w.username}, {"avatar_url", w.avatarUrl}};
    std::string body = to_string(bodyj);

    ix::HttpClient httpClient;
    if (!Config::certFile().empty()) {
        ix::SocketTLSOptions tlsOptions;
        tlsOptions.caFile = Config::certFile();
        httpClient.setTLSOptions(tlsOptions);
    }
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = {{"content-type", "application/json"}, {"origin", "localhost"}};
    auto res = httpClient.post(std::format("https://discord.com/api/webhooks/{}/{}", credentials.id(), credentials.secret()), body, args);
}