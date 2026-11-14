#include <vector>
#include <memory>
#include <random>
#include <utility>

#pragma once

#include "types.hpp"

class Treap {
public:
    // Public interface
    Treap();                                   // deterministic RNG seed
    explicit Treap(uint32_t seed);             // custom seed for reproducibility

    // Insert a key; returns true if insertion took place (no duplicate keys)
    bool insert(int key);

    // Erase a key; returns true if key was present and removed
    bool erase(int key);

    // Find a key; returns true if present
    bool find(int key) const;

    // Return keys in-order (sorted)
    std::vector<int> inorder() const;

    // Return number of nodes in the treap
    std::size_t size() const;

private:
    struct Node {
        int key;
        uint32_t priority;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        Node(int k, uint32_t p) : key(k), priority(p), left(nullptr), right(nullptr) {};
        Node() = default; // default ctor for container compatibility
    };

    std::unique_ptr<Node> root_;
    std::mt19937 rng_;

    // Helper recursive functions
    static std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>>
    split(std::unique_ptr<Node> current, int key);

    static std::unique_ptr<Node>
    merge(std::unique_ptr<Node> left, std::unique_ptr<Node> right);

    static bool find(const std::unique_ptr<Node>& node, int key);

    static void inorder(const std::unique_ptr<Node>& node, std::vector<int>& out);

    static std::size_t size(const std::unique_ptr<Node>& node);
};
