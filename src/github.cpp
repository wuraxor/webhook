#include <webhook/github.h>

github::User::User(const Json::Value& j) : name(j["name"].asString()), login(j["login"].asString()),
    avatarUrl(j["avatar_url"].asString()), htmlUrl(j["html_url"].asString()) {
}

github::Repository::Repository(const Json::Value& j) : name(j["name"].asString()), htmlUrl(j["html_url"].asString()),
    owner(j["owner"]) {
}

github::Issue::Issue(const Json::Value& j) : title(j["title"].asString()), htmlUrl(j["html_url"].asString()) {
}
