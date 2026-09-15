#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "../Common.hpp"
#include "../File.hpp"
#include "../Item.hpp"

/*
** How the cache file will be organized
** input last modification time | input size | 0-entry | ... | n-entry
*/

namespace IndexedSequencialAccess {
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
    explicit Cache(File& input);
    ~Cache();

private:
    std::ifstream file_;

    // Check existing cache file data with it input
    // the checked fields are last modification time and size
    bool ValidateCache(const File& input);

    bool TryLoadExistingCache(const std::string& cachePath, const File& input);
    void BuildCache(File& input, const std::string& cachePath);

    static int CreateSortedPageFiles(File& input);
    static void WriteSortedPageFile(const std::string& pagePath,
                                    const std::array<Item, PAGE_SIZE>& page,
                                    int pageIndex);

    static void MergePageFiles(const std::string& cachePath, const File& input,
                               int pageCount);
    static void CleanupPageFiles(const std::string& basePath, int pageCount);

    static std::string GetCachePath(const File& input);
    static std::string GetPagePath(const std::string& basePath, int pageIndex);
};
}  // namespace IndexedSequencialAccess
