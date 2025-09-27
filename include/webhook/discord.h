#pragma once
#include <string>

struct Webhook;

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
    static void Send(const std::string& event, const std::string& body, const Credentials& credentials);
private:
    static void Send(const Webhook& w, const Credentials& credentials);
};

struct Webhook {
    std::string username;
    std::string avatarUrl;
    std::string content;
};