#include "File.hpp"
#include "Common.hpp"
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
std::vector<Item> File::GetItems(int pag) {
    if(pag < 0){
        return {};
    }
    //verificar se a pag existe ou ultrapassa o tamanho do aquivo
    const std::streamoff offset = pag*sizeof(Item)*PAGE_SIZE; 
    if(static_cast<uint64_t>(offset) >= this->size()){
        return {};
    }
    
    std::vector<Item> items(PAGE_SIZE);

    this->file_.clear();
    this->file_.seekg(offset, std::ios::beg);//movimentando o ponteiro para a pag desejada
    this->file_.read(reinterpret_cast<char*>(items.data()), sizeof(Item)*PAGE_SIZE);//lendo os bytes de uma página
    
    std::streamsize bytesLidos = this->file_.gcount();
    items.resize(bytesLidos / sizeof(Item));
    return items;
}

std::string File::path() const { return this->path_; }

std::filesystem::file_time_type File::lastModification() const {
    return std::filesystem::last_write_time(this->path_);
}

uint64_t File::size() const { return std::filesystem::file_size(this->path_); }
