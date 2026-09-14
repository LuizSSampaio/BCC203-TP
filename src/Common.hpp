#pragma once

#include <string>

inline constexpr int PAGE_SIZE = 100;

class Log {
public:
    Log() = delete;

    static void Info(const std::string& msg);
    static void Error(const std::string& msg);

    inline static bool enableInfo = false;
};
