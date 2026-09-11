#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    constexpr int requiredArgC = 5;
    if (argc < requiredArgC) {
        std::cout
            << "Usage: pesquisa <method> <quantity> <situation> <key> [-P]";
    }

    constexpr int optionalLastArgPos = 5;
    if (argc == (optionalLastArgPos + 1) &&
        std::string(argv[optionalLastArgPos]) == std::string("-P")) {
        // TODO: Set verbose mode
    }

    return 0;
}
