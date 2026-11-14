#include <vector>
#include <cassert>
#include <algorithm>

#include <iostream>
#include "types.hpp"
#include "core.hpp"

void test_empty_tree() {
    Treap t;
    assert(t.size() == 0);
    assert(!t.find(10));
    assert(t.inorder().empty());
    assert(!t.erase(5)); // nothing to erase
}

void test_single_insertion() {
    Treap t;
    assert(t.insert(42));
    assert(t.size() == 1);
    assert(t.find(42));
    assert(!t.find(0));
    auto vec = t.inorder();
    assert(vec.size() == 1 && vec[0] == 42);
    // duplicate insertion should fail
    assert(!t.insert(42));
    assert(t.size() == 1);
}

void test_multiple_insertions() {
    Treap t;
    std::vector<int> keys = {5, 3, 8, 1, 4, 7, 9};
    for (int k : keys) {
        assert(t.insert(k));
    }
    assert(t.size() == keys.size());
    // Verify ordering
    auto vec = t.inorder();
    std::vector<int> sorted = keys;
    std::sort(sorted.begin(), sorted.end());
    assert(vec == sorted);
    // Verify find
    for (int k : keys) {
        assert(t.find(k));
    }
    assert(!t.find(100));
}

void test_erase_leaf() {
    Treap t;
    t.insert(10);
    t.insert(5);
    t.insert(15);
    assert(t.erase(5));
    assert(t.size() == 2);
    assert(!t.find(5));
    auto vec = t.inorder();
    assert((vec == std::vector<int>{10,15}));
}

void test_erase_internal_node() {
    Treap t;
    std::vector<int> keys = {20, 10, 30, 5, 15, 25, 35};
    for (int k : keys) t.insert(k);
    assert(t.erase(20)); // root may change
    assert(t.size() == keys.size() - 1);
    assert(!t.find(20));
    auto vec = t.inorder();
    std::vector<int> expected = {5,10,15,25,30,35};
    assert(vec == expected);
}

void test_randomized_operations() {
    Treap t(2021); // deterministic seed
    const int N = 1000;
    std::vector<int> inserted;
    for (int i = 0; i < N; ++i) {
        int val = i * 2; // even numbers
        assert(t.insert(val));
        inserted.push_back(val);
    }
    assert(t.size() == inserted.size());
    // Random deletions
    for (int i = 0; i < N; i += 3) {
        int val = i * 2;
        assert(t.erase(val));
    }
    // Verify remaining keys are the odds of the original sequence
    std::vector<int> expected;
    for (int i = 0; i < N; ++i) {
        if (i % 3 != 0) expected.push_back(i * 2);
    }
    auto vec = t.inorder();
    assert(vec == expected);
    // Verify find correctness
    for (int i = 0; i < N; ++i) {
        int val = i * 2;
        bool should_exist = (i % 3 != 0);
        assert(t.find(val) == should_exist);
    }
}

int main() {
    test_empty_tree();
    test_single_insertion();
    test_multiple_insertions();
    test_erase_leaf();
    test_erase_internal_node();
    test_randomized_operations();

    std::cout << "All Treap unit tests passed.\n";
    return 0;
}
