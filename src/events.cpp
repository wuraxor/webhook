#include <regex>
#include <webhook/events.h>

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

CheckRun::CheckRun(const Json::Value& j) : action(j["action"].asString()), checkRun(j["check_run"].asString()),
    repository(j["repository"].asString()) {
}

CheckRun::checkRun::checkRun(const Json::Value& j) : conclusion(j["conclusion"].asString()), htmlUrl(j["html_url"].asString()),
    app(j["app"]), checkSuite(j["check_suite"]) {
}

CheckRun::checkRun::app::app(const Json::Value& j) : name(j["name"].asString()) {
}

CheckRun::checkRun::checkSuite::checkSuite(const Json::Value& j) : headBranch(j["head_branch"].asString()) {
}

Fork::Fork(const Json::Value& j) : forkee(j["forkee"]), sender(j["sender"]) {
}

Fork::forkee::forkee(const Json::Value& j) : name(j["name"].asString()), htmlUrl(j["html_url"].asString()) {
}

Issues::Issues(const Json::Value& j) : action(j["action"].asString()), issue(j["issue"]), repository(j["repository"]),
    sender(j["sender"]){
}

IssueComment::IssueComment(const Json::Value& j) : action(j["action"].asString()), comment(j["comment"]), issue(j["issue"]),
    repository(j["repository"]), sender(j["sender"]) {
}

IssueComment::comment::comment(const Json::Value& j) : body(j["body"].asString()), htmlUrl(j["html_url"].asString()) {
}

Public::Public(const Json::Value& j) : repository(j["repository"]), sender(j["sender"]) {
}

PullRequest::PullRequest(const Json::Value& j) : action(j["action"].asString()), pullRequest(j["pull_request"]),
    repository(j["repository"]), sender(j["sender"]) {
}

Push::Push(const Json::Value& j) : created(j["created"].asBool()), deleted(j["deleted"].asBool()), forced(j["forced"].asBool()),
    ref(j["ref"].asString()), sender(j["sender"]), pusher(j["pusher"]), repository(j["repository"]) {
    for (const auto& commit : j["commits"]) {
        commits.emplace_back(commit);
    }
}

Push::commits::commits(const Json::Value& j) : id(j["id"].asString()), url(j["url"].asString()), message(j["message"].asString()) {
}

Release::Release(const Json::Value& j) : action(j["action"].asString()), release(j["release"]), sender(j["sender"]) {
}

Release::release::release(const Json::Value& j) : htmlUrl(j["html_url"].asString()), tagName(j["tag_name"].asString()) {
}

Repository::Repository(const Json::Value& j) : action(j["action"].asString()), changes(j["changes"]), repository(j["repository"]), sender(j["sender"]) {
}

Repository::changes::changes(const Json::Value& j) : repository(j["repository"]) {
}

Repository::changes::repository::repository(const Json::Value& j) : name(j["name"]) {
}

Repository::changes::repository::name::name(const Json::Value& j) : from(j["from"].asString()) {
}

Star::Star(const Json::Value& j) : action(j["action"].asString()), sender(j["sender"]), repository(j["repository"])  {
}

WorkflowRun::WorkflowRun(const Json::Value& j) : action(j["action"].asString()), workflow(j["workflow"]), workflowRun(j["workflow_run"]), repository(j["repository"]) {
}

WorkflowRun::workflow::workflow(const Json::Value& j) : name(j["name"].asString()) {
}

WorkflowRun::workflowRun::workflowRun(const Json::Value& j) : conclusion(j["conclusion"].asString()), htmlUrl(j["html_url"].asString()),
    headBranch(j["head_branch"].asString()) {
}

const static auto ignoredChecks = std::array<std::string, 3>{
    "Dependabot",
    "GitHub Actions",
    "GitHub Advanced Security",
};

std::optional<Webhook> CheckRunFunc(const Json::Value& j) {
    CheckRun e(j);
    if (e.action != "completed" || e.checkRun.conclusion.empty()) {
        return {};
    }

    if (std::ranges::find(ignoredChecks, e.checkRun.app.name) != ignoredChecks.end()) {
        return {};
    }

    return Webhook {
        e.checkRun.app.name,
        e.repository.owner.avatarUrl,
        std::format("Check [{}](<{}>) on [{}](<{}>)/[{}](<{}/tree/{}>)",
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

std::optional<Webhook> ForkFunc(const Json::Value& j) {
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

std::optional<Webhook> IssuesFunc(const Json::Value& j) {
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

std::optional<Webhook> IssueCommentFunc(const Json::Value& j) {
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

std::optional<Webhook> PublicFunc(const Json::Value& j) {
    Public e(j);
    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) made [{}](<{}>) public",
            e.sender.login,
            e.sender.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        )
    };
}

const static auto ignoredPullRequestActions = std::array<std::string, 2> {
    "labeled",
    "synchronize"
};

std::optional<Webhook> PullRequestFunc(const Json::Value& j) {
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

std::optional<Webhook> PushFunc(const Json::Value& j) {
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

std::optional<Webhook> ReleaseFunc(const Json::Value& j) {
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

std::optional<Webhook> RepositoryFunc(const Json::Value& j) {
    Repository e(j);
    if (std::ranges::find(supportedRepositoryActions, e.action)) {
        return {};
    }

    std::string content = std::format("[{}](<{}>) ", e.sender.login, e.sender.htmlUrl);

    if (e.action == "archived" || e.action == "unarchived") {
        content += std::format(
            "{} [{}](<{}>)",
            e.action,
            e.repository.name,
            e.repository.htmlUrl
        );
    } else if (e.action == "privatized") {
        content += std::format(
            "renamed [{}](<{}>) to [{}](<{}>)",
            e.changes.repository.name.from,
            e.repository.htmlUrl,
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

std::optional<Webhook> StarFunc(const Json::Value& j) {
    Star e(j);
    if (e.action != "created") {
        return {};
    }

    return Webhook{
        e.sender.login,
        e.sender.avatarUrl,
        std::format("[{}](<{}>) starred [{}](<{}>)",
            e.sender.login,
            e.sender.htmlUrl,
            e.repository.name,
            e.repository.htmlUrl
        )
    };
}

const static auto ignoredWorkflows = std::array<std::string, 3> {
    "CodeQL",
    "Dependabot Updates",
    "Automatic Dependency Submission",
};

std::optional<Webhook> WorkflowRunFunc(const Json::Value& j) {
    WorkflowRun e(j);
    if (e.action != "completed" || e.workflowRun.conclusion.empty()) {
        return {};
    }

    if (std::ranges::find(ignoredWorkflows, e.action) != ignoredWorkflows.end()) {
        return {};
    }

    return Webhook{
        e.workflow.name,
        e.repository.owner.avatarUrl,
        std::format("Workflow [{}](<{}>) on [{}](<{}>)/[{}](<{}/tree/{}>)",
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
