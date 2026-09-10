#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "Item.hpp"

class File {
public:
    File(const std::string& path);
    ~File();

    std::vector<Item> GetItems(int quantity);

private:
    std::ifstream file_;
};
