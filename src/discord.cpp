#include <utility>
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

Discord::Discord(const std::string& event, const std::string& body, const Credentials& credentials) {
    auto eventFunc = EventMap.at(event);
}
