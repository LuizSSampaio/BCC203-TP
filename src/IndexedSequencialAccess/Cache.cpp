#include "Cache.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>

#include "../Common.hpp"
#include "../File.hpp"
#include "../Item.hpp"

using namespace Algorithm::IndexedSequencialAccess;

Cache::Cache(File& input) {}

Cache::~Cache() {}

std::optional<Cache::Entry> Cache::Search(int key) {
    // TODO
    return {};
}

bool Cache::ValidateCache(const File& input) {
    // TODO
    return true;
}

bool Cache::TryLoadExistingCache(const std::string& cachePath,
                                 const File& input) {
    // TODO
    return true;
}

void Cache::BuildCache(File& input, const std::string& cachePath) {}

int Cache::CreateSortedPageFiles(File& input) {
    // TODO
    return 0;
}

void Cache::WriteSortedPageFile(const std::string& pagePath,
                                const std::array<Item, PAGE_SIZE>& page,
                                int pageIndex) {}

void Cache::MergePageFiles(const std::string& cachePath, const File& input,
                           int pageCount) {
    std::ofstream cacheFile(cachePath, std::ios::binary);
    if (!cacheFile.is_open()) {
        Log::Error("Falha ao criar o arquvio");
        return;
    }
    Metadata meta;
    meta.lastModification = input.lastModification();
    meta.size = input.size();
    // Escrevendo metadados no arquivo
    cacheFile.write(reinterpret_cast<const char*>(&meta), sizeof(Metadata));
    std::vector<Entry> entry(pageCount);
    std::vector<std::ifstream> pageFile(pageCount);
    std::priority_queue<Entry>;
    std::vector<Entry>;
    std::priority_queue<Entry>;
    std::greater<Entry>minHeap;

    for (int i = 0; i < pageCount; i++) {
        std::string pagePath = GetPagePath(cachePath, i);
        pageFile[i].open(pagePath, std::ios::binary);
        
        if (pageFile[i].is_open()) {
            // lê o primeiro item da pagina
            Item firstItem;
            if(pageFile[i].read(reinterpret_cast<char*>(&firstItem), sizeof(Item)){
                minHeap.push(firstItem, i)

            }
        }
    }
    int i;
    pageFile[i].close();
    std::sort(entry, {}, &Entry::key);  // ordenando as páginas
    // escrita na cache
    for(int i = 0; i<pageCount; i++){
        cacheFile.write(reinterpret_cast<const char*>(&entry[i]),
                    sizeof(Entry) * pageCount);
    }
    cacheFile.close();
}

void Cache::CleanupPageFiles(const std::string& basePath, int pageCount) {
    for (int i = 0; i < pageCount; i++) {
        std::string pagePath = GetPagePath(basePath, i);

        std::error_code ec;
        if (std::filesystem::exists(pagePath)) {
            std::filesystem::remove(pagePath, ec);
            if (ec) {
                Log::Error("Não foi  possível remover o arquivo!");
            }
        }
    }
}

std::string Cache::GetCachePath(const File& input) {
    // TODO
    return "TODO";
}

std::string Cache::GetPagePath(const std::string& basePath, int pageIndex) {
    // TODO
    return "TODO";
}
