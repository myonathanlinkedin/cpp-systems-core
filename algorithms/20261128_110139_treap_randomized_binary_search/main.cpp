#include <iostream>
#include <vector>
#include <cassert>
#include <random>

#include "types.hpp"
#include "engine.cpp"   // Include implementation directly for a single‑translation‑unit build

using namespace treap;

int main() {
    // Basic insertion and inorder check
    Treap<int, int> t;
    t.insert(5, 50);
    t.insert(3, 30);
    t.insert(7, 70);
    t.insert(1, 10);
    t.insert(9, 90);
    t.insert(4, 40);
    t.insert(6, 60);
    t.insert(8, 80);
    t.insert(2, 20);

    // Verify sorted order
    std::vector<int> expected = {1,2,3,4,5,6,7,8,9};
    assert(t.inorder() == expected);

    // Verify find operation
    const int* val = t.find(4);
    assert(val && *val == 40);
    assert(t.find(10) == nullptr);

    // Verify BST + heap invariants after random inserts
    assert(t.validate());

    // Erase leaf node
    assert(t.erase(1));
    expected = {2,3,4,5,6,7,8,9};
    assert(t.inorder() == expected);
    assert(t.validate());

    // Erase node with one child
    assert(t.erase(9));
    expected = {2,3,4,5,6,7,8};
    assert(t.inorder() == expected);
    assert(t.validate());

    // Erase node with two children
    assert(t.erase(5));
    expected = {2,3,4,6,7,8};
    assert(t.inorder() == expected);
    assert(t.validate());

    // Erase non‑existent key
    assert(!t.erase(100));

    // Stress test: insert many random keys and verify invariants
    Treap<int, int> stress;
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    const int N = 5000;
    for (int i = 0; i < N; ++i) {
        int k = dist(gen);
        stress.insert(k, k * 2);
        assert(stress.validate());
    }
    // Random deletions
    for (int i = 0; i < N / 2; ++i) {
        int k = dist(gen);
        stress.erase(k);
        assert(stress.validate());
    }

    // All tests passed
    std::cout << "All treap unit tests passed.\n";
    return 0;
}
