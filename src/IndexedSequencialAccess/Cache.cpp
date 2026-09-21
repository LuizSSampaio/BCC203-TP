#include "Cache.hpp"
#include "Common.hpp"
#include "File.hpp"
#include "Item.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>

namespace Algorithm::IndexedSequencialAccess{


bool Cache::ValidateCache(const File& input){
    
    std::string cachePath = GetCachePath(input);
    std::ifstream cacheFile(cachePath, std::ios::binary);

    if(!cacheFile.is_open()){
        Log::Error("Falha ao abrir a cache!");
        return false;
    }

    Metadata meta;
    cacheFile.read(reinterpret_cast<char*>(&meta), sizeof(Metadata));

    if(meta.lastModification == input.lastModification() && meta.size == input.size()){
        cacheFile.close();
        return true;
    }
    cacheFile.close();
    return false;
}
bool Cache::TryLoadExistingCache(const std::string& cachePath, const File& input){
    if(std::filesystem::exists(cachePath)){
        if(ValidateCache(input)){
            return true;
        }
    }
    return false;
}
void Cache::BuildCache(File& input, const std::string& cachePath){
    if(!TryLoadExistingCache(cachePath, input)){
        int pageCount = CreateSortedPageFiles(input);
        
        MergePageFiles(cachePath, input,pageCount);
        CleanupPageFiles(cachePath, pageCount);

        this->file_.open(cachePath, std::ios::binary);
    }
}
void Cache::MergePageFiles(const std::string& cachePath, const File& input,int pageCount){
    std::ofstream cacheFile(cachePath, std::ios::binary);
    if(!cacheFile.is_open()){
        Log::Error("Falha ao criar o arquvio");
        return;
    }
    Metadata meta;
    meta.lastModification = input.lastModification();
    meta.size = input.size();
    //Escrevendo metadados no arquivo
    cacheFile.write(reinterpret_cast<const char*>(&meta), sizeof(Metadata));
    std::vector<Entry> entry(pageCount); 
    for(int i = 0;i < pageCount; i++){
        std::string pagePath = GetPagePath(cachePath, i);
        std::ifstream pageFile(pagePath, std::ios::binary);
        if(pageFile.is_open()){
            //lê o primeiro item da pagina
            Item firstItem;
            pageFile.read(reinterpret_cast<char*>(&firstItem), sizeof(Item));
            //coloca os dados no vetor
            entry[i].key = firstItem.key;
            entry[i].pageIndex = static_cast<uint64_t>(i);
            
            pageFile.close();
        }
    }
    std::sort(entry.begin(), entry.end());//ordenando as páginas
    //escrita na cache
    cacheFile.write(reinterpret_cast<const char*>(&entry), sizeof(Entry)*pageCount);

    cacheFile.close();
}

void Cache::CleanupPageFiles(const std::string& basePath, int pageCount){
    for(int i = 0; i < pageCount; i++){
        std::string pagePath = GetPagePath(basePath, i);

        std::error_code ec;
        if(std::filesystem::exists(pagePath)){
            std::filesystem::remove(pagePath,ec);
            if(ec){
                Log::Error("Não foi  possível remover o arquivo!");
            }
        } 
    }
}






















}