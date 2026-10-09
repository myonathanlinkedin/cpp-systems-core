#include <memory>
#include <cassert>

#include "types.hpp"
#include <utility>
#include <algorithm>

template<typename Key>
std::pair<std::unique_ptr<BTreeNode<Key>>, Key> splitNode(std::unique_ptr<BTreeNode<Key>>& node) {
    assert(node);
    assert(node->keys.size() == static_cast<size_t>(2 * node->t - 1));
    int t = node->t;

    // Create right node
    auto right = std::make_unique<BTreeNode<Key>>(node->leaf, t);

    // Move keys to right node
    for (int j = t; j < 2 * t - 1; ++j) {
        right->keys.push_back(std::move(node->keys[j]));
    }

    // Middle key to promote
    Key midKey = std::move(node->keys[t - 1]);

    // Resize left node keys
    node->keys.resize(t - 1);

    // Handle children if not leaf
    if (!node->leaf) {
        for (int j = t; j < 2 * t; ++j) {
            right->children.push_back(std::move(node->children[j]));
        }
        node->children.resize(t);
    }

    return {std::move(right), std::move(midKey)};
}
