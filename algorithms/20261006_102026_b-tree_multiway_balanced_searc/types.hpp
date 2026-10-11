#pragma once
#include <vector>
#include <memory>
#include <cassert>

template<typename Key>
struct BTreeNode {
    bool leaf;
    std::vector<Key> keys;
    std::vector<std::unique_ptr<BTreeNode>> children;
    int t; // minimum degree

    BTreeNode(bool leaf_, int t_) : leaf(leaf_), t(t_) {};
};
