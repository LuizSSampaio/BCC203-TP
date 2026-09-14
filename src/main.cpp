#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include "Common.hpp"
#include "File.hpp"

enum Method : uint8_t {
    IndexedSequencialAccess = 1,
    BinaryTree = 2,
    BTree = 3,
    BStarTree = 4,
};

enum FileType : uint8_t {
    Ascending = 1,
    Descending = 2,
    Unordered = 3,
};

int main(int argc, char* argv[]) {
    constexpr int requiredArgC = 5;
    if (argc < requiredArgC) {
        Log::error
            << "Use pesquisa <method> <quantity> <situation> <key> [-P]\n";
        return -1;
    }

    Log::error.SetEnabled(true);

    constexpr int optionalLastArgPos = 5;
    if (argc == (optionalLastArgPos + 1) &&
        std::string(argv[optionalLastArgPos]) == std::string("-P")) {
        Log::info.SetEnabled(true);
    }

    long int const quantity = std::atol(argv[2]);
    constexpr int maxFileSize = 2000000;
    if (quantity > maxFileSize || quantity <= 0) {
        Log::error << "<quantity> must be a value grater than 0 and equal or "
                      "less than 2000000";
        return -1;
    }

    int const situationValue = std::atoi(argv[3]);
    if (situationValue < FileType::Ascending ||
        situationValue > FileType::Unordered) {
        Log::error << "Error: <situation> must be one of these: 1 -> ascending "
                      "file; 2 -> descending file; 3 -> unordered file\n";
        return -1;
    }

    std::string fileTypeName;
    switch (static_cast<FileType>(situationValue)) {
        case Ascending:
            fileTypeName = "items_ascending.bin";
            break;
        case Descending:
            fileTypeName = "items_descending.bin";
            break;
        case Unordered:
            fileTypeName = "items_shuffled.bin";
            break;
    }

    // std::filesystem is used because the old implementation had an issue with
    // the location of the executable
    // The generator create the file at ./tmp but the old code searched at
    // ./build/tmp
    // I've update it to handle these two cases.
    std::filesystem::path filePath = "tmp/" + fileTypeName;
    if (!std::filesystem::exists(filePath)) {
        const auto execPath = std::filesystem::path(argv[0]);
        const auto candidateParent =
            execPath.parent_path() / ".." / "tmp" / fileTypeName;
        const auto candidateSame =
            execPath.parent_path() / "tmp" / fileTypeName;

        if (std::filesystem::exists(candidateParent)) {
            filePath = candidateParent;
        } else if (std::filesystem::exists(candidateSame)) {
            filePath = candidateSame;
        }
    }
    File const file(filePath.string());

    int const key = std::atoi(argv[4]);
    if (key < 0 || key > quantity - 1) {
        Log::error
            << "Error: <key> must be greater or equal to 0 and less than "
               "<quantity>\n";
        return -1;
    }

    switch (std::atoi(argv[1])) {
        case Method::IndexedSequencialAccess:
            // TODO
            break;
        case Method::BinaryTree:
            // TODO
            break;
        case Method::BTree:
            // TODO
            break;
        case Method::BStarTree:
            // TODO
            break;
        default:
            Log::error << "Error: <method>, must be one of these: 1 -> indexed "
                          "sequencial access; 2 -> binary tree; 3 -> B tree; 4 "
                          "-> B* tree\n";
            return -1;
    }

    return 0;
}
