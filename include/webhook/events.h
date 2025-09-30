#pragma once
#include <webhook/discord.h>
#include <webhook/github.h>

struct CheckRun {
    explicit CheckRun(const nlohmann::json&);

    std::string action;

    struct checkRun {
        explicit checkRun(const nlohmann::json&);

        std::string conclusion;
        std::string htmlUrl;

        struct app {
            explicit app(const nlohmann::json&);
            std::string name;
        } app;

        struct checkSuite {
            explicit checkSuite(const nlohmann::json&);
            std::string headBranch;
        } checkSuite;

    } checkRun;

    github::Repository repository;
};

struct Fork {
    explicit Fork(const nlohmann::json&);

    struct forkee {
        explicit forkee(const nlohmann::json&);
        std::string name;
        std::string htmlUrl;
    } forkee;

    github::User sender;
};

struct Issues {
    explicit Issues(const nlohmann::json&);

    std::string action;
    github::Issue issue;
    github::Repository repository;
    github::User sender;
};

struct IssueComment {
    explicit IssueComment(const nlohmann::json&);

    std::string action;

    struct comment {
        explicit comment(const nlohmann::json&);
        std::string body;
        std::string htmlUrl;
    } comment;

    github::Issue issue;
    github::Repository repository;
    github::User sender;
};

struct Public {
    explicit Public(const nlohmann::json&);

    github::Repository repository;
    github::User sender;
};

struct PullRequest {
    explicit PullRequest(const nlohmann::json&);

    std::string action;
    github::Issue pullRequest;
    github::Repository repository;
    github::User sender;
};

struct Push {
    explicit Push(const nlohmann::json&);

    struct commits {
        explicit commits(const nlohmann::json&);
        std::string id;
        std::string url;
        std::string message;
    };

    std::vector<commits> commits;
    bool created;
    bool deleted;
    bool forced;
    std::string ref;
    github::User sender;
    github::User pusher;
    github::Repository repository;
};

struct Release {
    explicit Release(const nlohmann::json&);

    std::string action;

    struct release {
        explicit release(const nlohmann::json&);
        std::string htmlUrl;
        std::string tagName;
    } release;

    github::User sender;
};

struct Repository {
    explicit Repository(const nlohmann::json&);

    std::string action;

    struct changes {
        explicit changes(const nlohmann::json&);
        struct repository {
            explicit repository(const nlohmann::json&);
            struct name {
                explicit name(const nlohmann::json&);
                std::string from;
            } name;
        } repository;
    } changes;

    github::Repository repository;
    github::User sender;
};

struct Star {
    explicit Star(const nlohmann::json&);

    std::string action;
    github::User sender;
    github::Repository repository;
};

struct WorkflowRun {
    explicit WorkflowRun(const nlohmann::json&);

    std::string action;

    struct workflow {
        explicit workflow(const nlohmann::json&);

        std::string name;
    } workflow;

    struct workflowRun {
        explicit workflowRun(const nlohmann::json&);

        std::string conclusion;
        std::string htmlUrl;
        std::string headBranch;
    } workflowRun;

    github::Repository repository;
};

std::optional<Webhook> CheckRunFunc(const nlohmann::json&);

std::optional<Webhook> ForkFunc(const nlohmann::json&);

std::optional<Webhook> IssuesFunc(const nlohmann::json&);

std::optional<Webhook> IssueCommentFunc(const nlohmann::json&);

std::optional<Webhook> PublicFunc(const nlohmann::json&);

std::optional<Webhook> PullRequestFunc(const nlohmann::json&);

std::optional<Webhook> PushFunc(const nlohmann::json&);

std::optional<Webhook> ReleaseFunc(const nlohmann::json&);

std::optional<Webhook> RepositoryFunc(const nlohmann::json&);

std::optional<Webhook> StarFunc(const nlohmann::json&);

std::optional<Webhook> WorkflowRunFunc(const nlohmann::json&);