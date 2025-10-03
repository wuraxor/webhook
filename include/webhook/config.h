#pragma once
#include <string>


class Config {
public:
    static void load();

    static const std::string& hostname();
    static unsigned short port();
    static const std::string& happyEmoji();
    static const std::string& successEmoji();
    static const std::string& failureEmoji();
    static const std::string& certFile();
private:
    inline static std::string hostname_;
    inline static unsigned short port_;
    inline static std::string happyEmoji_;
    inline static std::string successEmoji_;
    inline static std::string failureEmoji_;
    inline static std::string certFile_;
};
