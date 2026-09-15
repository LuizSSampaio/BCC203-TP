#include "File.hpp"
#define ITENS_PAG 100
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
    if(pag < 0)
        return {};
    //verificar se a pag existe ou ultrapassa o tamanho do aquivo
    
    std::vector<Item> items(ITENS_PAG);

    this->file_.clear();
    this->file_.seekg(pag*sizeof(Item)*ITENS_PAG, std::ios::beg);//movimentando o ponteiro para a pag desejada
    this->file_.read(reinterpret_cast<char*>(items.data()), sizeof(Item)*ITENS_PAG);//lendo os bytes de uma página
    
    std::streamsize bytesLidos = this->file_.gcount();
    items.resize(bytesLidos / sizeof(Item));
    return items;
}

std::string File::path() const { return this->path_; }

std::filesystem::file_time_type File::lastModification() const {
    return std::filesystem::last_write_time(this->path_);
}

uint64_t File::size() const { return std::filesystem::file_size(this->path_); }
