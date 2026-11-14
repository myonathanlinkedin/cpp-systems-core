#include <vector>
#include <memory>
#include <utility>

#include "core.hpp"

Treap::Treap() : root_(nullptr), rng_(12345u) {}

Treap::Treap(uint32_t seed) : root_(nullptr), rng_(seed) {}

bool Treap::insert(int key) {
    if (find(key)) return false; // no duplicates
    uint32_t prio = rng_();
    auto new_node = std::make_unique<Node>(key, prio);
    auto splitted = split(std::move(root_), key);
    root_ = merge(merge(std::move(splitted.first), std::move(new_node)), std::move(splitted.second));
    return true;
}

bool Treap::erase(int key) {
    if (!find(key)) return false;
    // Split into <key, >=key
    auto left_mid = split(std::move(root_), key);
    // left_mid.second has keys >= key; split it into ==key and >key
    auto mid_right = split(std::move(left_mid.second), key + 1);
    // mid_right.first is the node with key (or empty if duplicate handling)
    // Discard mid_right.first (the node to delete)
    root_ = merge(std::move(left_mid.first), std::move(mid_right.second));
    return true;
}

bool Treap::find(int key) const {
    return find(root_, key);
}

std::vector<int> Treap::inorder() const {
    std::vector<int> out;
    inorder(root_, out);
    return out;
}

std::size_t Treap::size() const {
    return size(root_);
}

// ---------- Private static helpers ----------

std::pair<std::unique_ptr<Treap::Node>, std::unique_ptr<Treap::Node>>
Treap::split(std::unique_ptr<Node> current, int key) {
    if (!current) {
        return {nullptr, nullptr};
    }
    if (key <= current->key) {
        // All keys >= key go to right part
        auto splitted = split(std::move(current->left), key);
        current->left = std::move(splitted.second);
        return {std::move(splitted.first), std::move(current)};
    } else {
        // All keys < key go to left part
        auto splitted = split(std::move(current->right), key);
        current->right = std::move(splitted.first);
        return {std::move(current), std::move(splitted.second)};
    }
}

std::unique_ptr<Treap::Node>
Treap::merge(std::unique_ptr<Node> left, std::unique_ptr<Node> right) {
    if (!left) return right;
    if (!right) return left;
    if (left->priority < right->priority) {
        left->right = merge(std::move(left->right), std::move(right));
        return left;
    } else {
        right->left = merge(std::move(left), std::move(right->left));
        return right;
    }
}

bool Treap::find(const std::unique_ptr<Node>& node, int key) {
    const Node* cur = node.get();
    while (cur) {
        if (key == cur->key) return true;
        cur = (key < cur->key) ? cur->left.get() : cur->right.get();
    }
    return false;
}

void Treap::inorder(const std::unique_ptr<Node>& node, std::vector<int>& out) {
    if (!node) return;
    inorder(node->left, out);
    out.push_back(node->key);
    inorder(node->right, out);
}

std::size_t Treap::size(const std::unique_ptr<Node>& node) {
    if (!node) return 0;
    return 1 + size(node->left) + size(node->right);
}
