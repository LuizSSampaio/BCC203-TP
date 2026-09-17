#pragma once

#include <array>
#include <ostream>

struct Item {
    static constexpr int textSize = 5000;

    int key;
    long int value;
    std::array<char, textSize> text;
};

inline std::ostream& operator<<(std::ostream& strm, const Item& item) {
    return strm << "{ key = " << item.key << "; value = " << item.value
                << "; text = \""
                << std::string_view(item.text.data(), item.text.size())
                << "\"; }";
}
