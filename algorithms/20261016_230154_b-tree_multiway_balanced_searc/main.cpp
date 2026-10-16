#include <vector>
#include <algorithm>
#include <functional>

#include <iostream>
#include <cassert>
#include "types.hpp"
#include "core.cpp"

int main() {
    // Test 1: Basic split on root
    {
        BTree tree(3); // Minimum degree 3
        for (int i = 1; i <= 5; ++i) tree.insert(i);
        // Root should be full now
        assert(tree.root->keys.size() == 5);
        tree.insert(6); // Trigger split
        // After split, root should have 1 key
        assert(tree.root->keys.size() == 1);
        assert(tree.root->keys[0] == 3);
        // Left child
        assert(tree.root->children[0]->keys.size() == 2);
        assert(tree.root->children[0]->keys[0] == 1);
        assert(tree.root->children[0]->keys[1] == 2);
        // Right child
        assert(tree.root->children[1]->keys.size() == 3);
        assert(tree.root->children[1]->keys[0] == 4);
        assert(tree.root->children[1]->keys[1] == 5);
        assert(tree.root->children[1]->keys[2] == 6);
    }

    // Test 2: Split a non-leaf child
    {
        BTree tree(2); // Minimum degree 2
        // Insert keys to create a node that will split
        for (int i = 1; i <= 3; ++i) tree.insert(i); // root has 3 keys
        // Insert 4 to trigger split of root
        tree.insert(4);
        // Now root has 1 key, two children
        assert(tree.root->keys.size() == 1);
        assert(tree.root->keys[0] == 2);
        // Left child keys
        assert(tree.root->children[0]->keys.size() == 1);
        assert(tree.root->children[0]->keys[0] == 1);
        // Right child keys
        assert(tree.root->children[1]->keys.size() == 2);
        assert(tree.root->children[1]->keys[0] == 3);
        assert(tree.root->children[1]->keys[1] == 4);
    }

    // Test 3: Insert many keys and verify order
    {
        BTree tree(3);
        std::vector<int> vals = {10, 20, 5, 6, 12, 30, 7, 17};
        for (int v : vals) tree.insert(v);
        // After insertions, perform an in-order traversal to verify sorted order
        std::vector<int> inorder;
        std::function<void(BTreeNode*)> traverse = [&](BTreeNode* node) {
            for (size_t i = 0; i < node->keys.size(); ++i) {
                if (!node->leaf) traverse(node->children[i].get());
                inorder.push_back(node->keys[i]);
            }
            if (!node->leaf) traverse(node->children.back().get());
        };
        traverse(tree.root.get());
        std::vector<int> sorted = vals;
        std::sort(sorted.begin(), sorted.end());
        assert(inorder == sorted);
    }

    std::cout << "All tests passed.\n";
    return 0;
}
