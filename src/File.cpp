#include "File.hpp"

#include <stdexcept>

File::File(const std::string& path) {
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
