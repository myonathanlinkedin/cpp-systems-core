#include <algorithm>

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include "types.hpp"

int main() {
    // Test 1: Direct node split on leaf node
    {
        BTreeNode<int> node;
        node.keys = {1, 2, 3, 4, 5}; // t = 3, 2t-1 = 5 keys
        assert(node.isFull(3));
        auto [promoted, right] = node.split(3);
        assert(promoted == 3);
        assert(node.keys.size() == 2);
        assert(node.keys[0] == 1 && node.keys[1] == 2);
        assert(right->keys.size() == 2);
        assert(right->keys[0] == 4 && right->keys[1] == 5);
        assert(node.isLeaf() && right->isLeaf());
    }

    // Test 2: Direct node split on internal node
    {
        BTreeNode<int> node;
        node.keys = {10, 20, 30, 40, 50};
        // Create 6 children
        for (int i = 0; i < 6; ++i) {
            node.children.push_back(std::make_unique<BTreeNode<int>>());
            node.children.back()->keys = {i * 5};
        }
        assert(node.isFull(3));
        auto [promoted, right] = node.split(3);
        assert(promoted == 30);
        assert(node.keys.size() == 2);
        assert(node.keys[0] == 10 && node.keys[1] == 20);
        assert(right->keys.size() == 2);
        assert(right->keys[0] == 40 && right->keys[1] == 50);
        assert(node.children.size() == 3);
        assert(right->children.size() == 3);
        // Verify children distribution
        for (int i = 0; i < 3; ++i) {
            assert(node.children[i]->keys[0] == i * 5);
            assert(right->children[i]->keys[0] == (i + 3) * 5);
        }
    }

    // Test 3: BTree insertion and search
    {
        BTree<int> tree(3); // order t = 3
        std::vector<int> values = {15, 5, 25, 10, 20, 30, 35, 1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 24, 26, 27, 28, 29, 31, 32, 33, 34, 36, 37, 38, 39, 40};
        for (int v : values) {
            tree.insert(v);
        }
        // Verify all inserted values are searchable
        for (int v : values) {
            assert(tree.search(v));
        }
        // Verify non-existent values are not found
        for (int v = 0; v <= 40; ++v) {
            if (std::find(values.begin(), values.end(), v) == values.end()) {
                assert(!tree.search(v));
            }
        }
    }

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
