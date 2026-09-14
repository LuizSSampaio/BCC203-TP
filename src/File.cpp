#include "File.hpp"

#include <filesystem>
#include <stdexcept>

File::File(const std::string& path) : path_(path) {
    this->file_ = std::ifstream(path, std::ios::binary);

    if (!this->file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

File::~File() { this->file_.close(); }

// TODO: add reading by page
std::vector<Item> File::GetItems(int quantity) {
    std::vector<Item> items;
    items.reserve(quantity);

    for (int i = 0; i < quantity; i++) {
        Item item;
        this->file_.read(reinterpret_cast<char*>(&item), sizeof(Item));
        items.push_back(item);
    }

    return items;
}

std::string File::path() const { return this->path_; }

std::filesystem::file_time_type File::lastModification() const {
    return std::filesystem::last_write_time(this->path_);
}

uint64_t File::size() const { return std::filesystem::file_size(this->path_); }
