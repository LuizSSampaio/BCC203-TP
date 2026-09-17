#pragma once

#include "../File.hpp"
#include "../Item.hpp"
#include "BTreeFile.hpp"

namespace Algorithm::BinaryTree {
class BTree {
public:
    explicit BTree(File& input);
    ~BTree() = default;

    std::optional<Item> Search(int key);

private:
    BTreeFile file_;
};
}  // namespace Algorithm::BinaryTree
