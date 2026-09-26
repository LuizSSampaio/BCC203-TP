#include "Cache.hpp"
#include "File.hpp"
#include "Item.hpp"

using namespace Algorithm::IndexedSequentialAccess;

void Cache::BuildCache(File& input, const std::string& cachePath){
    std::ofstream cacheFile(cachePath, std::ios::binary);
    if(!cacheFile.is_open()){
        Log::Error("Não foi possível gerar a cache!");
        return;
    }
    //Escrevendo dados sobre os últimos acessos
    Metadata meta;
    meta.lastModification = input.lastModification();
    meta.size = input.size();
    cacheFile.write(reinterpret_cast<char*>(&meta), sizeof(Metadata));


    size_t index = 0;
    std::array<Item, PAGE_SIZE> page;

    Entry tmp;
    while(true){
        //Lê a proxima página
        page = input.GetNextPage();
        if (input.eof()) {
            break;
        }
        //Grava a entrada no arquivo de cache
        tmp.key = page[0].key;
        tmp.pageIndex = index;
        
        cacheFile.write(reinterpret_cast<char*>(&tmp),sizeof(Entry) );
        index++;
    }
    cacheFile.close();
}