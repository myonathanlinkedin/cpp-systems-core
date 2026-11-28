#pragma once

#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>

namespace treap {

// Forward declaration
template <typename K, typename V>
class Treap;

// Node structure for the treap
template <typename K, typename V>
struct Node {
    K key;
    V value;
    uint32_t priority;                     // Heap priority (lower value = higher priority)
    std::unique_ptr<Node> left;            // Left subtree
    std::unique_ptr<Node> right;           // Right subtree

    // Default constructor required for containers
    Node() = default;

    // Parameterized constructor
    Node(const K& k, const V& v, uint32_t p)
        : key(k), value(v), priority(p), left(nullptr), right(nullptr) {};
};

} // namespace treap
