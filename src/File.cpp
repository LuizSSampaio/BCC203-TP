#include "File.hpp"

#include <filesystem>
#include <stdexcept>

#include "Common.hpp"

File::File(const std::string& path, uint64_t quantity)
    : path_(path), quantity_(quantity) {
    this->file_ = std::ifstream(path, std::ios::binary);

    if (!this->file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

File::~File() { this->file_.close(); }

// TODO: make page reading in a single operation
std::array<Item, PAGE_SIZE> File::GetNextPage() {
    std::array<Item, PAGE_SIZE> page;

    for (int i = 0; i < PAGE_SIZE; i++) {
        Item item;
        this->file_.read(reinterpret_cast<char*>(&item), sizeof(Item));
        page[i] = item;
    }

    return page;
}

// TODO: Implement edge case checks
std::array<Item, PAGE_SIZE> File::GetPageAt(size_t index) {
    std::array<Item, PAGE_SIZE> page;
    auto oldPos = this->file_.tellg();

    if (this->quantity_ / PAGE_SIZE >= index || index < 0) {
        Log::Error("Invalid input file access index");
        return page;
    }

    this->file_.seekg(sizeof(Item) * PAGE_SIZE * index, std::ifstream::beg);
    page = this->GetNextPage();

    this->file_.seekg(oldPos);
    return page;
}

std::string File::path() const { return this->path_; }

std::filesystem::file_time_type File::lastModification() const {
    return std::filesystem::last_write_time(this->path_);
}

uint64_t File::size() const { return std::filesystem::file_size(this->path_); }

uint64_t File::quantity() const { return this->quantity_; }
