#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

inline constexpr int PAGE_SIZE = 100;

namespace Log {

class LogStream {
public:
    LogStream(bool enabled, std::string_view prefix, std::string_view color,
              std::ostream& out = std::cout);
    ~LogStream();

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;

    LogStream(LogStream&& other) noexcept;
    LogStream& operator=(LogStream&& other) noexcept;

    template <typename T>
    LogStream& operator<<(const T& val) {
        if (enabled_) {
            buffer_ << val;
        }
        return *this;
    }

    LogStream& operator<<(std::ostream& (*manip)(std::ostream&));

private:
    bool enabled_;
    std::string_view prefix_;
    std::string_view color_;
    std::ostream& out_;
    std::ostringstream buffer_;
};

class Logger {
public:
    Logger(std::string_view prefix, std::string_view color,
           bool enabled = false, std::ostream& out = std::cout);

    [[nodiscard]] bool enabled() const;
    void SetEnabled(bool value);

    template <typename T>
    LogStream operator<<(const T& val) const {
        LogStream stream(enabled_, prefix_, color_, out_);
        stream << val;
        return stream;
    }

    LogStream operator<<(std::ostream& (*manip)(std::ostream&)) const;

private:
    std::string_view prefix_;
    std::string_view color_;
    bool enabled_;
    std::ostream& out_;
};

extern Logger Info;
extern Logger Error;

inline Logger& info = Info;
inline Logger& error = Error;

}  // namespace Log
