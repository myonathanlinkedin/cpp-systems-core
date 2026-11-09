#include <iostream>
#include <vector>
#include <cassert>

#include "types.hpp"
#include "engine.cpp"   // Pull in implementation; ensures linkage without a separate header

using namespace treearch;

int main() {
    // -----------------------------------------------------------------------
    // Basic construction and sanity checks
    // -----------------------------------------------------------------------
    Tree t(10); // root value = 10
    assert(t.size() == 1);
    assert(t.root_id() == 0);
    assert(t.node(0).value == 10);
    assert(t.depth(0) == 0);
    assert(t.height(0) == 0);

    // -----------------------------------------------------------------------
    // Build a non‑trivial tree:
    //          10
    //        /  |  \
    //       5   3   8
    //      / \       \
    //     2   7       9
    // -----------------------------------------------------------------------
    NodeId n5 = t.add_child(t.root_id(), 5);
    NodeId n3 = t.add_child(t.root_id(), 3);
    NodeId n8 = t.add_child(t.root_id(), 8);
    NodeId n2 = t.add_child(n5, 2);
    NodeId n7 = t.add_child(n5, 7);
    NodeId n9 = t.add_child(n8, 9);

    // Verify size and parent relationships
    assert(t.size() == 7);
    assert(t.node(n5).parent == t.root_id());
    assert(t.node(n2).parent == n5);
    assert(t.node(n9).parent == n8);

    // Depth checks
    assert(t.depth(t.root_id()) == 0);
    assert(t.depth(n5) == 1);
    assert(t.depth(n2) == 2);
    assert(t.depth(n9) == 2);

    // Height checks
    // Subtree rooted at node 5 has height 1 (children 2 and 7 are leaves)
    assert(t.height(n5) == 1);
    // Subtree rooted at node 8 has height 1 (child 9)
    assert(t.height(n8) == 1);
    // Whole tree height is 2 (root -> 5 -> 2)
    assert(t.height(t.root_id()) == 2);

    // -----------------------------------------------------------------------
    // Traversal correctness
    // -----------------------------------------------------------------------
    std::vector<int> expected_preorder   = {10, 5, 2, 7, 3, 8, 9};
    std::vector<int> expected_postorder  = {2, 7, 5, 3, 9, 8, 10};
    std::vector<int> expected_levelorder = {10, 5, 3, 8, 2, 7, 9};

    assert(t.preorder() == expected_preorder);
    assert(t.postorder() == expected_postorder);
    assert(t.level_order() == expected_levelorder);

    // -----------------------------------------------------------------------
    // Edge case: single‑node tree after re‑initialisation
    // -----------------------------------------------------------------------
    Tree single(42);
    assert(single.size() == 1);
    assert(single.preorder() == std::vector<int>{42});
    assert(single.postorder() == std::vector<int>{42});
    assert(single.level_order() == std::vector<int>{42});
    assert(single.depth(single.root_id()) == 0);
    assert(single.height(single.root_id()) == 0);

    // -----------------------------------------------------------------------
    // Stress test: linear chain (depth = size-1, height = size-1)
    // -----------------------------------------------------------------------
    Tree chain(0);
    NodeId prev = chain.root_id();
    const std::size_t chain_len = 1000;
    for (std::size_t i = 1; i < chain_len; ++i) {
        prev = chain.add_child(prev, static_cast<int>(i));
    }
    assert(chain.size() == chain_len);
    assert(chain.depth(prev) == chain_len - 1);
    assert(chain.height(chain.root_id()) == chain_len - 1);
    // Verify that preorder and postorder of a chain are identical (root first, then leaves)
    std::vector<int> preorder_chain = chain.preorder();
    std::vector<int> postorder_chain = chain.postorder();
    assert(preorder_chain.front() == 0);
    assert(preorder_chain.back() == static_cast<int>(chain_len - 1));
    assert(postorder_chain.back() == 0);
    assert(postorder_chain.front() == static_cast<int>(chain_len - 1));

    // All assertions passed
    std::cout << "All tree architecture tests passed successfully.\n";
    return 0;
}
