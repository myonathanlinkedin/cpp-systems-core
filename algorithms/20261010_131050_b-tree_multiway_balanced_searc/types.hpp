#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>
#include <cassert>

template<typename Key>
struct BTreeNode {
    std::vector<Key> keys;
    std::vector<std::unique_ptr<BTreeNode>> children;

    BTreeNode() = default;
    BTreeNode(const BTreeNode&) = delete;
    BTreeNode& operator=(const BTreeNode&) = delete;
    BTreeNode(BTreeNode&&) = default;
    BTreeNode& operator=(BTreeNode&&) = default;

    bool isLeaf() const { return children.empty(); };
    bool isFull(size_t t) const { return keys.size() == 2 * t - 1; }

    // Split a full node into two nodes and promote the middle key.
    // Precondition: this node is full (has 2t-1 keys).
    std::pair<Key, std::unique_ptr<BTreeNode>> split(size_t t) {
        assert(isFull(t));
        size_t mid = t - 1;
        Key promoted = keys[mid];

        auto right = std::make_unique<BTreeNode>();
        // Move keys after mid to right node
        right->keys.assign(keys.begin() + mid + 1, keys.end());
        // Move children after mid+1 to right node if not leaf
        if (!isLeaf()) {
            right->children.assign(std::make_move_iterator(children.begin() + mid + 1),
                                   std::make_move_iterator(children.end()));
        }
        // Resize current node to keep left side
        keys.resize(mid);
        if (!isLeaf()) {
            children.resize(mid + 1);
        }
        return {promoted, std::move(right)};
    }
};

template<typename Key>
class BTree {
public:
    explicit BTree(size_t t) : t_(t), root_(std::make_unique<BTreeNode<Key>>()) {};

    void insert(const Key& key) {
        if (root_->isFull(t_)) {
            auto newRoot = std::make_unique<BTreeNode<Key>>();
            newRoot->children.push_back(std::move(root_));
            splitChild(newRoot.get(), 0);
            root_ = std::move(newRoot);
        }
        insertNonFull(root_.get(), key);
    }

    bool search(const Key& key) const {
        return searchRecursive(root_.get(), key);
    }

private:
    size_t t_;
    std::unique_ptr<BTreeNode<Key>> root_;

    void insertNonFull(BTreeNode<Key>* node, const Key& key) {
        size_t i = node->keys.size();
        if (node->isLeaf()) {
            // Insert key into the leaf node in sorted order
            node->keys.push_back(key);
            std::inplace_merge(node->keys.begin(), node->keys.end() - 1, node->keys.end());
        } else {
            // Find child to descend into
            while (i > 0 && key < node->keys[i - 1]) {
                --i;
            }
            if (node->children[i]->isFull(t_)) {
                splitChild(node, i);
                if (key > node->keys[i]) {
                    ++i;
                }
            }
            insertNonFull(node->children[i].get(), key);
        }
    }

    void splitChild(BTreeNode<Key>* parent, size_t index) {
        auto child = std::move(parent->children[index]);
        auto [promoted, right] = child->split(t_);
        parent->keys.insert(parent->keys.begin() + index, promoted);
        parent->children.insert(parent->children.begin() + index + 1, std::move(right));
        parent->children[index] = std::move(child);
    }

    bool searchRecursive(const BTreeNode<Key>* node, const Key& key) const {
        size_t i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            ++i;
        }
        if (i < node->keys.size() && key == node->keys[i]) {
            return true;
        }
        if (node->isLeaf()) {
            return false;
        }
        return searchRecursive(node->children[i].get(), key);
    }
};
