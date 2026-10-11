#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

namespace btree {

template<typename Key>
struct Node {
    bool leaf;
    std::vector<Key> keys;
    std::vector<std::unique_ptr<Node>> children;
    Node(bool leaf = true) : leaf(leaf) {};
};

template<typename Key>
class BTree {
public:
    explicit BTree(int t);
    void insert(const Key& k);
    bool search(const Key& k) const;
    int getT() const { return t; };
    const Node<Key>* getRoot() const { return root.get(); }
private:
    int t; // minimum degree
    std::unique_ptr<Node<Key>> root;
    void splitChild(Node<Key>* x, int i);
    void insertNonFull(Node<Key>* x, const Key& k);
    bool search(const Node<Key>* x, const Key& k) const;
};

} // namespace btree
