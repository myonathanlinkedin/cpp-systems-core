#include <iostream>
#include <vector>
#include <cassert>
#include "types.hpp"

using namespace btree;

// Helper function to validate B-Tree properties
bool validateNode(const Node<int>* node, int t, bool isRoot) {
    // Keys must be sorted
    for (size_t i = 1; i < node->keys.size(); ++i) {
        assert(node->keys[i - 1] <= node->keys[i]);
    }

    // Key count constraints
    if (isRoot) {
        assert(node->keys.size() <= static_cast<size_t>(2 * t - 1));
    } else {
        assert(node->keys.size() >= static_cast<size_t>(t - 1));
        assert(node->keys.size() <= static_cast<size_t>(2 * t - 1));
    }

    // Children count constraints
    if (node->leaf) {
        assert(node->children.empty());
    } else {
        assert(node->children.size() == node->keys.size() + 1);
        for (const auto& child : node->children) {
            assert(child);
            validateNode(child.get(), t, false);
        }
    }
    return true;
}

bool validateBTree(const BTree<int>& tree) {
    const Node<int>* root = tree.getRoot();
    assert(root);
    return validateNode(root, tree.getT(), true);
}

int main() {
    // Test 1: Basic insertion and search
    {
        BTree<int> tree(3); // minimum degree 3
        for (int i = 1; i <= 10; ++i) {
            tree.insert(i);
        }
        for (int i = 1; i <= 10; ++i) {
            assert(tree.search(i));
        }
        assert(!tree.search(0));
        assert(!tree.search(11));
        assert(validateBTree(tree));
    }

    // Test 2: Multiple splits
    {
        BTree<int> tree(3);
        for (int i = 1; i <= 20; ++i) {
            tree.insert(i);
        }
        for (int i = 1; i <= 20; ++i) {
            assert(tree.search(i));
        }
        assert(!tree.search(0));
        assert(!tree.search(21));
        assert(validateBTree(tree));
    }

    // Test 3: Root split
    {
        BTree<int> tree(2); // smaller degree to trigger splits quickly
        for (int i = 1; i <= 7; ++i) {
            tree.insert(i);
        }
        // After inserting 7 keys with t=2, root should have split
        const Node<int>* root = tree.getRoot();
        assert(root->keys.size() <= 3); // 2*t-1 = 3
        assert(root->keys.size() >= 1); // root can have fewer keys
        assert(validateBTree(tree));
    }

    // Test 4: Edge case - single insertion
    {
        BTree<int> tree(3);
        tree.insert(42);
        assert(tree.search(42));
        assert(!tree.search(43));
        assert(validateBTree(tree));
    }

    // Test 5: Duplicate insertion (allowed by current implementation)
    {
        BTree<int> tree(3);
        tree.insert(5);
        tree.insert(5);
        assert(tree.search(5));
        // Tree may contain duplicate keys; we don't enforce uniqueness
        assert(validateBTree(tree));
    }

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
