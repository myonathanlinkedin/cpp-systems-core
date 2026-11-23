#include <numeric>

#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <limits>
#include <cassert>
#include <iostream>

namespace treap {

// ---------------------------------------------------------------------------
// Node: a single element of the randomized BST (treap).
//
// Invariants:
//   BST  : left subtree keys < key < right subtree keys
//   Heap : priority >= every priority in its subtree (max-heap on priority)
//
// The node is owned by the treap via std::unique_ptr; children are raw
// pointers because the parent always outlives the children (ownership flows
// downward).  This keeps the tree trivially movable and avoids double-free.
// ---------------------------------------------------------------------------
struct Node {
    int64_t key;
    uint32_t priority;   // larger = closer to root (max-heap)
    int32_t size;        // subtree size (for rank / order-statistics)
    Node* left;
    Node* right;

    Node(int64_t k, uint32_t p)
        : key(k), priority(p), size(1), left(nullptr), right(nullptr) {};
};

// ---------------------------------------------------------------------------
// Treap: a randomized binary search tree with heap priorities.
//
// Complexity (expected, over the random priorities):
//   insert / erase / find / rank / select : O(log n)
//   in-order traversal                    : O(n)
//   construction from sorted array        : O(n)  (Cartesian-tree build)
//
// Space: O(n) nodes + O(log n) recursion depth (expected).
// ---------------------------------------------------------------------------
class Treap {
public:
    Treap();
    ~Treap();

    // Non-copyable, movable.
    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;
    Treap(Treap&& other) noexcept;
    Treap& operator=(Treap&& other) noexcept;

    // ---- Mutators ---------------------------------------------------------
    // Insert key.  Returns true if the key was newly inserted, false if it
    // already existed (no duplicate keys are stored).
    bool insert(int64_t key);

    // Erase key.  Returns true if the key was present and removed.
    bool erase(int64_t key);

    // ---- Queries ----------------------------------------------------------
    bool contains(int64_t key) const;
    int64_t findMin() const;   // smallest key, or std::numeric_limits<int64_t>::max()
    int64_t findMax() const;   // largest key,  or std::numeric_limits<int64_t>::min()
    int32_t size() const;

    // Rank: number of keys strictly less than `key`.  O(log n).
    int32_t rank(int64_t key) const;

    // Select: the k-th smallest key (0-indexed).  Returns
    // std::numeric_limits<int64_t>::min() if k is out of range.
    int64_t select(int32_t k) const;

    // ---- Traversal --------------------------------------------------------
    // In-order (ascending) keys.
    std::vector<int64_t> inOrder() const;

    // ---- Structural verification (used by tests) --------------------------
    // Returns true iff the tree satisfies BOTH the BST invariant and the
    // max-heap invariant on priorities, and every node's `size` field is
    // correct.
    bool verifyInvariants() const;

    // Height of the tree (0 for empty, 1 for a single node).
    int32_t height() const;

    // ---- Bulk construction -------------------------------------------------
    // Build a treap from an arbitrary (unsorted) vector of keys in O(n)
    // expected time using the Cartesian-tree construction.  Duplicates are
    // ignored.
    static Treap fromKeys(const std::vector<int64_t>& keys);

    // ---- Deterministic RNG access (for reproducible tests) ----------------
    void seed(uint32_t s);

private:
    Node* root_;
    uint32_t rngState_;

    // ---- Recursive helpers (operate on Node* subtrees) ---------------------
    static void updateSize(Node* n);
    static int32_t subtreeSize(const Node* n);
    static int32_t subtreeHeight(const Node* n);

    // Split subtree `t` into (lo, hi) where lo contains keys < key and
    // hi contains keys >= key.  Both results are valid treaps.
    static void split(Node* t, int64_t key, Node*& lo, Node*& hi);

    // Merge two treaps `a` and `b` where every key in a < every key in b.
    // Returns the root of the merged treap.
    static Node* merge(Node* a, Node* b);

    // Recursive insert: returns new root of the subtree.
    Node* insertRec(Node* t, int64_t key, bool& inserted);

    // Recursive erase: returns new root of the subtree.
    Node* eraseRec(Node* t, int64_t key, bool& erased);

    // Recursive contains.
    static bool containsRec(const Node* t, int64_t key);

    // Recursive rank (count keys < key).
    static int32_t rankRec(const Node* t, int64_t key);

    // Recursive select (k-th smallest, 0-indexed).
    static int64_t selectRec(const Node* t, int32_t k);

    // Recursive in-order collection.
    static void inOrderRec(const Node* t, std::vector<int64_t>& out);

    // Invariant check on a subtree.
    static bool verifyRec(const Node* t, int64_t lo, int64_t hi);

    // Cartesian-tree build: given keys sorted ascending and their priorities,
    // build the treap in O(n) using a monotone stack.
    static Node* buildCartesian(const std::vector<int64_t>& sortedKeys,
                                const std::vector<uint32_t>& priorities);

    // Deterministic xorshift32 PRNG.
    uint32_t nextRand();
};

} // namespace treap
