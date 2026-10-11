#pragma once
#include <vector>
#include <algorithm>
#include <cassert>

namespace btree {

template<typename K>
struct BTreeNode {
    bool leaf;
    std::vector<K> keys;
    std::vector<BTreeNode*> children;
    BTreeNode(bool leaf_) : leaf(leaf_) {};
    ~BTreeNode() {
        for (auto child : children) delete child;
    }
};

template<typename K>
class BTree {
public:
    explicit BTree(int t_) : t(t_), root(new BTreeNode<K>(true)) {};
    ~BTree() { delete root; }

    void insert(const K& k);
    bool search(const K& k) const;

private:
    int t; // minimum degree
    BTreeNode<K>* root;

    void splitChild(BTreeNode<K>* parent, int idx);
    void insertNonFull(BTreeNode<K>* node, const K& k);
};

} // namespace btree
