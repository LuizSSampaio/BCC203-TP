#include "Cache.hpp"
#include "Common.hpp"
#include "File.hpp"
#include "Item.hpp"
#include <fstream>
#include <filesystem>

namespace Algorithm::IndexedSequencialAccess{

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
    cacheFile.write(reinterpret_cast<char*>(&meta), sizeof(Metadata));

    for(int i = 0;i < pageCount; i++){
        std::string pagePath = GetPagePath(cachePath, i);
        std::ifstream pageFile(pagePath, std::ios::binary);
        if(pageFile.is_open()){
            //lê o primeiro item da pagina
            Item firstItem;
            pageFile.read(reinterpret_cast<char*>(&firstItem), sizeof(Item));
            //coloca os dados no entrada
            Entry entry;
            entry.key = firstItem.key;
            entry.pageIndex = static_cast<uint64_t>(i);
            //escreve os dados na cache
            cacheFile.write(reinterpret_cast<char*>(&entry), sizeof(Entry));
            pageFile.close();
        }
    }
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