#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Item.hpp"

class File {
public:
    File(const std::string& path);
    ~File();

    std::vector<Item> GetItems(int);

    std::string path() const;
    std::filesystem::file_time_type lastModification() const;
    uint64_t size() const;

private:
    std::ifstream file_;
    std::string path_;
};
