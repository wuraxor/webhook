#pragma once
#include <string>
#include <json/value.h>

namespace github {
    struct User {
        std::string name;
        std::string login;
        std::string avatarUrl;
        std::string htmlUrl;
        User(const Json::Value&);
    };

    struct Repository {
        std::string name;
        std::string htmlUrl;
        User owner;
        Repository(const Json::Value&);
    };

    struct Issue {
        std::string title;
        std::string htmlUrl;
        Issue(const Json::Value&);
    };
}
