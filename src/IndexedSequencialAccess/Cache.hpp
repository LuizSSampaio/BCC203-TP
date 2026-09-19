#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "../Common.hpp"
#include "../File.hpp"
#include "../Item.hpp"

/*
** How the cache file will be organized
** input last modification time | input size | 0-entry | ... | n-entry
*/

namespace Algorithm::IndexedSequencialAccess {
class Cache {
public:
    struct Entry {
        int key;
        uint64_t pageIndex;
    };

    struct Metadata {
        std::filesystem::file_time_type lastModification;
        uint64_t size;
    };

    // The explicity keyword is "optional", if removed the change is that the
    // compilar can explicity convert a File into a Cache
    // 1
    explicit Cache(File& input);
    // 1
    ~Cache();

    // 1
    std::optional<Entry> Search(int key);

private:
    std::ifstream file_;

    // Check existing cache file data with it input
    // the checked fields are last modification time and size
    // 1
    bool ValidateCache(const File& input);

    // 1
    bool TryLoadExistingCache(const std::string& cachePath, const File& input);
    // 2
    void BuildCache(File& input, const std::string& cachePath);

    // 2
    static int CreateSortedPageFiles(File& input);
    // 2
    static void WriteSortedPageFile(const std::string& pagePath,
                                    const std::array<Item, PAGE_SIZE>& page,
                                    int pageIndex);

    // 3
    static void MergePageFiles(const std::string& cachePath, const File& input,int pageCount);
    // 3
    static void CleanupPageFiles(const std::string& basePath, int pageCount);

    // 1
    static std::string GetCachePath(const File& input);
    // 1
    static std::string GetPagePath(const std::string& basePath, int pageIndex);
};
}  // namespace Algorithm::IndexedSequencialAccess
