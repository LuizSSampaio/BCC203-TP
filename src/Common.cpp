#include "Common.hpp"

#include <iostream>
#include <string>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

void Log::Info(const std::string& msg) {
    if (!Log::enableInfo) {
        return;
    }

    std::cout << BOLD << GREEN << "INFO: " << RESET << msg;
    if (msg.empty() || msg.back() != '\n') {
        std::cout << '\n';
    }
}
void Log::Error(const std::string& msg) {
    std::cout << BOLD << RED << "ERROR" << RESET << msg;
    if (msg.empty() || msg.back() != '\n') {
        std::cout << '\n';
    }
}
