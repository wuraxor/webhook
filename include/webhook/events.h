#pragma once
#include <json/value.h>
#include <webhook/discord.h>
#include <webhook/github.h>

struct CheckRun {
    explicit CheckRun(const Json::Value&);

    std::string action;

    struct checkRun {
        explicit checkRun(const Json::Value&);

        std::string conclusion;
        std::string htmlUrl;

        struct app {
            explicit app(const Json::Value&);
            std::string name;
        } app;

        struct checkSuite {
            explicit checkSuite(const Json::Value&);
            std::string headBranch;
        } checkSuite;

    } checkRun;

    github::Repository repository;
};

struct Fork {
    explicit Fork(const Json::Value&);

    struct forkee {
        explicit forkee(const Json::Value&);
        std::string name;
        std::string htmlUrl;
    } forkee;

    github::User sender;
};

struct Issues {
    explicit Issues(const Json::Value&);

    std::string action;
    github::Issue issue;
    github::Repository repository;
    github::User sender;
};

struct IssueComment {
    explicit IssueComment(const Json::Value&);

    std::string action;

    struct comment {
        explicit comment(const Json::Value&);
        std::string body;
        std::string htmlUrl;
    } comment;

    github::Issue issue;
    github::Repository repository;
    github::User sender;
};

struct Public {
    explicit Public(const Json::Value&);

    github::Repository repository;
    github::User sender;
};

struct PullRequest {
    explicit PullRequest(const Json::Value&);

    std::string action;
    github::Issue pullRequest;
    github::Repository repository;
    github::User sender;
};

struct Push {
    explicit Push(const Json::Value&);

    struct commits {
        explicit commits(const Json::Value&);
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
    explicit Release(const Json::Value&);

    std::string action;

    struct release {
        explicit release(const Json::Value&);
        std::string htmlUrl;
        std::string tagName;
    } release;

    github::User sender;
};

struct Repository {
    explicit Repository(const Json::Value&);

    std::string action;

    struct changes {
        explicit changes(const Json::Value&);
        struct repository {
            explicit repository(const Json::Value&);
            struct name {
                explicit name(const Json::Value&);
                std::string from;
            } name;
        } repository;
    } changes;

    github::Repository repository;
    github::User sender;
};

struct Star {
    explicit Star(const Json::Value&);

    std::string action;
    github::User sender;
    github::Repository repository;
};

struct WorkflowRun {
    explicit WorkflowRun(const Json::Value&);

    std::string action;

    struct workflow {
        explicit workflow(const Json::Value&);

        std::string name;
    } workflow;

    struct workflowRun {
        explicit workflowRun(const Json::Value&);

        std::string conclusion;
        std::string htmlUrl;
        std::string headBranch;
    } workflowRun;

    github::Repository repository;
};

std::optional<Webhook> CheckRunFunc(const Json::Value&);

std::optional<Webhook> ForkFunc(const Json::Value&);

std::optional<Webhook> IssuesFunc(const Json::Value&);

std::optional<Webhook> IssueCommentFunc(const Json::Value&);

std::optional<Webhook> PublicFunc(const Json::Value&);

std::optional<Webhook> PullRequestFunc(const Json::Value&);

std::optional<Webhook> PushFunc(const Json::Value&);

std::optional<Webhook> ReleaseFunc(const Json::Value&);

std::optional<Webhook> RepositoryFunc(const Json::Value&);

std::optional<Webhook> StarFunc(const Json::Value&);

std::optional<Webhook> WorkflowRunFunc(const Json::Value&);