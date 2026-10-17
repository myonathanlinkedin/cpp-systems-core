#include <memory>

#include "types.hpp"
#include "engine.cpp"
#include <iostream>
#include <cassert>

int main() {
    // Test 1: Leaf node split
    {
        int t = 3;
        auto node = std::make_unique<BTreeNode<int>>(true, t);
        for (int i = 1; i <= 2 * t - 1; ++i) {
            node->keys.push_back(i);
        }
        auto [right, promoted] = splitNode(node);
        assert(promoted == 3);
        assert(node->keys.size() == static_cast<size_t>(t - 1));
        assert(right->keys.size() == static_cast<size_t>(t - 1));
        assert(node->keys[0] == 1 && node->keys[1] == 2);
        assert(right->keys[0] == 4 && right->keys[1] == 5);
        assert(node->children.empty());
        assert(right->children.empty());
    }

    // Test 2: Internal node split
    {
        int t = 3;
        auto node = std::make_unique<BTreeNode<int>>(false, t);
        for (int i = 1; i <= 2 * t - 1; ++i) {
            node->keys.push_back(i);
        }
        // Create 2t children
        for (int i = 0; i < 2 * t; ++i) {
            auto child = std::make_unique<BTreeNode<int>>(true, t);
            child->keys.push_back(10 + i);
            node->children.push_back(std::move(child));
        }
        auto [right, promoted] = splitNode(node);
        assert(promoted == 3);
        assert(node->keys.size() == static_cast<size_t>(t - 1));
        assert(right->keys.size() == static_cast<size_t>(t - 1));
        assert(node->keys[0] == 1 && node->keys[1] == 2);
        assert(right->keys[0] == 4 && right->keys[1] == 5);
        assert(node->children.size() == static_cast<size_t>(t));
        assert(right->children.size() == static_cast<size_t>(t));
        for (int i = 0; i < t; ++i) {
            assert(node->children[i]->keys[0] == 10 + i);
            assert(right->children[i]->keys[0] == 10 + (t + i));
        }
    }

    // Test 3: Minimum degree 2
    {
        int t = 2;
        auto node = std::make_unique<BTreeNode<int>>(true, t);
        for (int i = 1; i <= 2 * t - 1; ++i) {
            node->keys.push_back(i);
        }
        auto [right, promoted] = splitNode(node);
        assert(promoted == 2);
        assert(node->keys.size() == static_cast<size_t>(t - 1));
        assert(right->keys.size() == static_cast<size_t>(t - 1));
        assert(node->keys[0] == 1);
        assert(right->keys[0] == 3);
    }

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
