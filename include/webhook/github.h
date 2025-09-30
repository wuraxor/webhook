#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace github {
    struct User {
        std::string name;
        std::string login;
        std::string avatarUrl;
        std::string htmlUrl;
        User(const nlohmann::json&);
    };

    struct Repository {
        std::string name;
        std::string htmlUrl;
        User owner;
        Repository(const nlohmann::json&);
    };

    struct Issue {
        std::string title;
        std::string htmlUrl;
        Issue(const nlohmann::json&);
    };
}
