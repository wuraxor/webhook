#include <iostream>
#include <webhook/config.h>

static std::string env(const char* env, std::string defaultValue = "") {
    return std::getenv(env) ? std::getenv(env) : defaultValue;
}

void Config::load() {
    auto addr = env("ADDR", ":8080");
    hostname_ = addr.substr(0, addr.find(":"));
    if (hostname_.empty()) {
        hostname_ = "0.0.0.0";
    }
    port_ = std::stoul(addr.substr(addr.find(":") + 1));
    happyEmoji_ = env("HAPPY_EMOJI", "🥳");
    successEmoji_ = env("SUCCESS_EMOJI", "🥖");
    failureEmoji_ = env("FAILURE_EMOJI", "😭");
    certFile_ = env("CERT_FILE", "");
}

const std::string& Config::hostname() {
    return hostname_;
}

unsigned short Config::port() {
    return port_;
}

const std::string& Config::happyEmoji() {
    return happyEmoji_;
}

const std::string& Config::successEmoji() {
    return successEmoji_;
}

const std::string& Config::failureEmoji() {
    return failureEmoji_;
}

const std::string& Config::certFile() {
    return certFile_;
}
