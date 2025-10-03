#include <regex>
#include <webhook/events.h>
#include <nlohmann/json.hpp>
#include <webhook/config.h>

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

const static auto linkRe = std::regex(R"(\[([^]]+)]\((https?://[^)]+)\))");

CheckRun::CheckRun(const nlohmann::json& j) : action(j.at("action").get<std::string>()),
                                              checkRun(j.at("check_run").get<std::string>()),
                                              repository(j.at("repository").get<std::string>()) {
}

CheckRun::checkRun::checkRun(const nlohmann::json& j) : conclusion(j.at("conclusion").get<std::string>()),
                                                        htmlUrl(j.at("html_url").get<std::string>()),
                                                        app(j.at("app")), checkSuite(j.at("check_suite")) {
}

CheckRun::checkRun::app::app(const nlohmann::json& j) : name(j.at("name").get<std::string>()) {
}

CheckRun::checkRun::checkSuite::checkSuite(const nlohmann::json& j) : headBranch(j.at("head_branch").get<std::string>()) {
}

Fork::Fork(const nlohmann::json& j) : forkee(j.at("forkee")), sender(j.at("sender")) {
}

Fork::forkee::forkee(const nlohmann::json& j) : name(j.at("name").get<std::string>()),
                                                htmlUrl(j.at("html_url").get<std::string>()) {
}

Issues::Issues(const nlohmann::json& j) : action(j.at("action").get<std::string>()), issue(j.at("issue")),
                                          repository(j.at("repository")),
                                          sender(j.at("sender")){
}

IssueComment::IssueComment(const nlohmann::json& j) : action(j.at("action").get<std::string>()),
                                                      comment(j.at("comment")), issue(j.at("issue")),
                                                      repository(j.at("repository")), sender(j.at("sender")) {
}

IssueComment::comment::comment(const nlohmann::json& j) : body(j.at("body").get<std::string>()),
                                                          htmlUrl(j.at("html_url").get<std::string>()) {
}

Public::Public(const nlohmann::json& j) : repository(j.at("repository")), sender(j.at("sender")) {
}

PullRequest::PullRequest(const nlohmann::json& j) : action(j.at("action").get<std::string>()),
                                                    pullRequest(j.at("pull_request")), repository(j.at("repository")),
                                                    sender(j.at("sender")) {
}

Push::Push(const nlohmann::json& j) : created(j.at("created").get<bool>()), deleted(j.at("deleted").get<bool>()),
                                      forced(j.at("forced").get<bool>()), ref(j.at("ref").get<std::string>()),
                                      sender(j.at("sender")), pusher(j.at("pusher")), repository(j.at("repository")) {
    for (const auto& commit : j.at("commits")) {
        commits.emplace_back(commit);
    }
}

Push::commits::commits(const nlohmann::json& j) : id(j.at("id").get<std::string>()),
                                                  url(j.at("url").get<std::string>()),
                                                  message(j.at("message").get<std::string>()) {
}

Release::Release(const nlohmann::json& j) : action(j.at("action").get<std::string>()), release(j.at("release")),
                                            sender(j.at("sender")) {
}

Release::release::release(const nlohmann::json& j) : htmlUrl(j.at("html_url").get<std::string>()),
                                                     tagName(j.at("tag_name").get<std::string>()) {
}

Repository::Repository(const nlohmann::json& j) : action(j.at("action").get<std::string>()), changes(j.at("changes")),
                                                  repository(j.at("repository")), sender(j.at("sender")) {
}

Repository::changes::changes(const nlohmann::json& j) : repository(j.at("repository")) {
}

Repository::changes::repository::repository(const nlohmann::json& j) : name(j.at("name")) {
}

Repository::changes::repository::name::name(const nlohmann::json& j) : from(j.at("from").get<std::string>()) {
}

Star::Star(const nlohmann::json& j) : action(j.at("action").get<std::string>()), sender(j.at("sender")),
                                      repository(j.at("repository")) {
}

WorkflowRun::WorkflowRun(const nlohmann::json& j) : action(j.at("action").get<std::string>()),
                                                    workflow(j.at("workflow")), workflowRun(j.at("workflow_run")),
                                                    repository(j.at("repository")) {
}

WorkflowRun::workflow::workflow(const nlohmann::json& j) : name(j.at("name").get<std::string>()) {
}

WorkflowRun::workflowRun::workflowRun(const nlohmann::json& j) : conclusion(j.at("conclusion").get<std::string>()),
                                                                 htmlUrl(j.at("html_url").get<std::string>()),
                                                                 headBranch(j.at("head_branch").get<std::string>()) {
}

const static auto ignoredChecks = std::array<std::string, 3>{
    "Dependabot",
    "GitHub Actions",
    "GitHub Advanced Security",
};

std::optional<Webhook> CheckRunFunc(const nlohmann::json& j) {
    CheckRun e(j);
    if (e.action != "completed" || e.checkRun.conclusion.empty()) {
        return {};
    }

    if (std::ranges::find(ignoredChecks, e.checkRun.app.name) != ignoredChecks.end()) {
        return {};
    }

    auto emoji = Config::successEmoji();
    if (e.checkRun.conclusion == "failure") {
        emoji = Config::failureEmoji();
    }

    return Webhook {
        e.checkRun.app.name,
        e.repository.owner.avatarUrl,
        std::format("{} Check [{}](<{}>) on [{}](<{}>)/[{}](<{}/tree/{}>)",
            emoji,
            e.checkRun.conclusion,
            e.checkRun.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl,
            e.checkRun.checkSuite.headBranch,
            e.repository.htmlUrl,
            e.checkRun.checkSuite.headBranch
        ),
    };
}

std::optional<Webhook> ForkFunc(const nlohmann::json& j) {
    Fork e(j);
    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) forked [{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.forkee.name,
            e.forkee.htmlUrl
        )
    };
}

std::optional<Webhook> IssuesFunc(const nlohmann::json& j) {
    Issues e(j);
    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) {} issue [{}](<{}>) in [{}](<{}>)/[{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.action,
            e.issue.title,
            e.issue.htmlUrl,
            e.repository.owner.login,
            e.repository.owner.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        )
    };
}

std::optional<Webhook> IssueCommentFunc(const nlohmann::json& j) {
    IssueComment e(j);
    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) {} comment on issue [{}](<{}>) in [{}](<{}>)/[{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.action,
            e.issue.title,
            e.issue.htmlUrl,
            e.repository.owner.login,
            e.repository.owner.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        )
    };
}

std::optional<Webhook> PublicFunc(const nlohmann::json& j) {
    Public e(j);
    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) made [{}](<{}>) public {}",
            e.sender.login,
            e.sender.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl,
            Config::happyEmoji()
        )
    };
}

const static auto ignoredPullRequestActions = std::array<std::string, 2> {
    "labeled",
    "synchronize"
};

std::optional<Webhook> PullRequestFunc(const nlohmann::json& j) {
    PullRequest e(j);
    if (e.action.contains("_")) {
        return {};
    }

    if (std::ranges::find(ignoredPullRequestActions, e.action) != ignoredPullRequestActions.end()) {
        return {};
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) {} pull request [{}](<{}>) in [{}](<{}>)/[{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.action,
            e.pullRequest.title,
            e.pullRequest.htmlUrl,
            e.repository.owner.login,
            e.repository.owner.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        )
    };
}

std::optional<Webhook> PushFunc(const nlohmann::json& j) {
    Push e(j);
    if (!e.ref.starts_with("refs/heads")) {
        return {};
    }

    std::string action = "";
    if (e.created) {
        if (e.forced) {
            action = " force";
        }
    } else if (e.deleted) {
        action = " deleted branch";
    } else if (e.forced) {
        action = " force pushed";
    }

    auto branch = e.ref.substr(11);

    auto footer = std::format("\n- [{}](<{}>){} on [{}](<{}>)/[{}](<{}/tree/{}>)",
        e.pusher.name,
        e.sender.htmlUrl,
        action,
        e.repository.name,
        e.repository.htmlUrl,
        branch,
        e.repository.htmlUrl,
        branch,
        branch
    );

    if (e.commits.empty()) {
        return Webhook{
            e.sender.login,
            e.sender.avatarUrl,
            footer.substr(1)
        };
    }

    std::string commits;
    for (const auto& c : e.commits) {
        auto lines = split(c.message, "\n");
        std::string first = "";
        if (!lines.empty()) {
            first = lines.front();
        }

        auto commitMsg = first;
        if (lines.size() > 1) {
            commitMsg += "...";
        }

        auto cleanMsg = std::regex_replace(commitMsg, linkRe, "[$1](<$2>)");

        commits += std::format("[`{}`](<{}>) {}\n", c.id.substr(0, 7), c.url, cleanMsg);
    }

    auto limit = 2000 - (footer.length() + std::string("...").length() + 1);
    if (commits.length() > limit) {
        auto truncated = commits.substr(0, limit) + "...";
        if (truncated.ends_with(">)")) {
            auto lines = split(truncated, "\n");
            if (lines.size() > 1) {
                truncated.clear();
                for (const auto str : lines) {
                    truncated += str + "\n";
                }
            }
        }
        commits = truncated + "\n";
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        commits + footer
    };
}

std::optional<Webhook> ReleaseFunc(const nlohmann::json& j) {
    Release e(j);
    if (e.action != "published") {
        return {};
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format(
            "[{}](<{}>) published release [{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.release.tagName,
            e.release.htmlUrl
        )
    };
}

const static auto supportedRepositoryActions = std::array<std::string, 4> {
    "archived",
    "privatized",
    "renamed",
    "unarchived"
};

std::optional<Webhook> RepositoryFunc(const nlohmann::json& j) {
    Repository e(j);
    if (std::ranges::find(supportedRepositoryActions, e.action)) {
        return {};
    }

    std::string content = std::format("[{}](<{}>) ", e.sender.login, e.sender.htmlUrl);

    if (e.action == "archived" || e.action == "unarchived") {
        std::string emoji;
        if (e.action == "archived") {
            emoji = "😭";
        } else {
            emoji = Config::happyEmoji();
        }
        content += std::format(
            "{} [{}](<{}>) {}",
            e.action,
            e.repository.name,
            e.repository.htmlUrl,
            emoji
        );
    } else if (e.action == "privatized") {
        content += std::format(
            "made [{}](<{}>) private 🖕",
            e.repository.name,
            e.repository.htmlUrl
        );
    } else if (e.action == "renamed") {
        content += std::format(
            "renamed [{}](<{}>) to [{}](<{}>)",
            e.changes.repository.name.from,
            e.repository.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        );
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        content
    };
}

std::optional<Webhook> StarFunc(const nlohmann::json& j) {
    Star e(j);
    if (e.action != "created") {
        return {};
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) starred [{}](<{}>) {}",
            e.sender.login,
            e.sender.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl,
            Config::happyEmoji()
        )
    };
}

const static auto ignoredWorkflows = std::array<std::string, 3> {
    "CodeQL",
    "Dependabot Updates",
    "Automatic Dependency Submission",
};

std::optional<Webhook> WorkflowRunFunc(const nlohmann::json& j) {
    WorkflowRun e(j);
    if (e.action != "completed" || e.workflowRun.conclusion.empty()) {
        return {};
    }

    if (std::ranges::find(ignoredWorkflows, e.action) != ignoredWorkflows.end()) {
        return {};
    }

    auto emoji = Config::successEmoji();
    if (e.workflowRun.conclusion == "failure") {
        emoji = Config::failureEmoji();
    }

    return Webhook{
        e.workflow.name,
        e.repository.owner.avatarUrl,
        std::format("{} Workflow [{}](<{}>) on [{}](<{}>)/[{}](<{}/tree/{}>)",
            emoji,
            e.workflowRun.conclusion,
            e.workflowRun.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl,
            e.workflowRun.headBranch,
            e.workflowRun.htmlUrl,
            e.workflowRun.headBranch
        )
    };
}
