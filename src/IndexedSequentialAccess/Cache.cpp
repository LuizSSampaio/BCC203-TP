#include "Cache.hpp"
#include <iostream>
#include <stdexcept>


namespace Algorithm::IndexedSequentialAccess {

Cache::Cache(File& input){
    std::string const cachePath = GetCachePath(input);

    if(!TryLoadExistingCache(cachePath, input)){
        BuildCache(input,cachePath);
    }
}


Cache::~Cache(){
    if(this->file_.is_open()){
        this->file_.close();
    }
}

bool Cache::ValidateCache(const File& input){
//reposiciona o ponteiro pro inicio do arquivo de cache
    this->file_.seekg(0,std::ifstream::beg);

//le e armazena os metadados do disco(tamanho e lastmodification)
    Metadata m;
    
    if(!this->file_.read((char*)&m, sizeof(Metadata))){
    this->file_.clear();
    return false;
    }
//variaveis que verificam se o cache tem os mesmos metadados
    bool sameModificationTime = (m.lastModification == input.lastModification());

    bool sameSize = (m.size == input.size());

//retorna verdadeiro se eles tem os mesmos metadados, falso se nao
    return sameModificationTime && sameSize;
}

bool Cache::TryLoadExistingCache(const std::string& cachePath, const File& input){
    if(!std::filesystem::exists(cachePath)) return false;

    this->file_.open(cachePath, std::ios::binary);

    if(!this->file_.is_open()) return false;

    if(!this->ValidateCache(input)){
        this->file_.close();
        return false;
    }
    
    return true;
}

std::string Cache::GetCachePath(const File&input){
    return input.path() + ".cache_01";

}

std::optional<Cache::Entry> Cache::Search(int key){

//carregado pela primeira vez, o vetor estara vazio
    if(this->entries_.empty()){
        if(!this->file_.is_open()){
            return std::nullopt;
        }
//posiciona o ponteiro pra logo apos os metadados
        this->file_.seekg(sizeof(Metadata), std::ifstream::beg);
        Entry entry;
//le as entradas e coloca no fim do vetor de indices
        while(this->file_.read((char*)&entry,sizeof(Entry))){
            this->entries_.push_back(entry);
        }

        this->file_.clear();
    }
//faz a busca propriamente dita
    size_t i = 0;
    size_t const tam = this->entries_.size();

    while(i < tam && this->entries_[i].key <= key){
        i++;
    }
//a chave eh menor que a chave do primeiro indice
    if (i == 0){
        return std::nullopt;
    }

    return this->entries_[i-1];

}


}//namespace Algorithm::IndexedSequentialAcess