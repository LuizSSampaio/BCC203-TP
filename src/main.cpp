#include <iostream>

int main(int argc, char* /*argv*/[]) {
    constexpr int requiredArgC = 5;
    if (argc < requiredArgC) {
        std::cout
            << "Usage: pesquisa <method> <quantity> <situation> <key> [-P]";
    }

    return 0;
}
