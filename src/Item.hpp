#pragma once

#include <array>

struct Item {
    static constexpr int textSize = 5000;

    int key;
    long int value;
    std::array<char, textSize> text;
};
