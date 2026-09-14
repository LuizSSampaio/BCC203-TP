#include "Common.hpp"

#include <iostream>
#include <utility>

#define RESET "\033[0m"

namespace Log {

LogStream::LogStream(bool enabled, std::string_view prefix,
                     std::string_view color, std::ostream& out)
    : enabled_(enabled), prefix_(prefix), color_(color), out_(out) {}

LogStream::~LogStream() {
    if (enabled_) {
        std::string const content = buffer_.str();
        out_ << color_ << prefix_ << RESET << content;
        if (content.empty() || content.back() != '\n') {
            out_ << '\n';
        }
    }
}

LogStream::LogStream(LogStream&& other) noexcept
    : enabled_(other.enabled_),
      prefix_(other.prefix_),
      color_(other.color_),
      out_(other.out_),
      buffer_(std::move(other.buffer_)) {
    other.enabled_ = false;
}

LogStream& LogStream::operator=(LogStream&& other) noexcept {
    if (this != &other) {
        enabled_ = other.enabled_;
        prefix_ = other.prefix_;
        color_ = other.color_;
        buffer_ = std::move(other.buffer_);
        other.enabled_ = false;
    }
    return *this;
}

LogStream& LogStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (enabled_) {
        manip(buffer_);
    }
    return *this;
}

Logger::Logger(std::string_view prefix, std::string_view color, bool enabled,
               std::ostream& out)
    : prefix_(prefix), color_(color), enabled_(enabled), out_(out) {}

bool Logger::enabled() const { return enabled_; }

void Logger::SetEnabled(bool value) { enabled_ = value; }

LogStream Logger::operator<<(std::ostream& (*manip)(std::ostream&)) const {
    LogStream stream(enabled_, prefix_, color_, out_);
    stream << manip;
    return stream;
}

Logger Info{"INFO: ", "\033[1m\033[32m", false, std::cout};
Logger Error{"ERROR: ", "\033[1m\033[31m", true, std::cout};

}  // namespace Log
