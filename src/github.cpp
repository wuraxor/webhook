#include <webhook/github.h>

github::User::User(const nlohmann::json& j) {
    if (j.contains("name")) {
        name = j.at("name").get<std::string>();
    }
    if (j.contains("avatar_url")) {
        avatarUrl = j.at("avatar_url").get<std::string>();
    }
    if (j.contains("login")) {
        login = j.at("login").get<std::string>();
    }
    if (j.contains("html_url")) {
        htmlUrl = j.at("html_url").get<std::string>();
    }
}

github::Repository::Repository(const nlohmann::json& j) : name(j.at("name").get<std::string>()), htmlUrl(j.at("html_url").get<std::string>()),
    owner(j.at("owner")) {
}

github::Issue::Issue(const nlohmann::json& j) : title(j.at("title").get<std::string>()), htmlUrl(j.at("html_url").get<std::string>()) {
}
