#pragma once
#include <string>

struct Credentials {
    Credentials(std::string  id, std::string  secret);
    [[nodiscard]] const std::string& id() const;
    [[nodiscard]] const std::string& secret() const;
private:
    const std::string _id;
    const std::string _secret;
};


class Discord {
public:
    Discord(const std::string& event, const std::string& body, const Credentials& credentials);
};

struct Webhook {
    std::string username;
    std::string avatarUrl;
    std::string content;
};