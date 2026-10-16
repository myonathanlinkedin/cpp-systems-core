#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include <memory>
#include <functional>

namespace btree {

// ---------------------------------------------------------------------------
// B-Tree invariants (min degree t):
//   * Every non-leaf node has between t and 2t children.
//   * Every leaf has between t-1 and 2t-1 keys.
//   * Every internal node with k keys has exactly k+1 children.
//   * All leaves are at the same depth.
//   * Keys are strictly increasing within a node.
//
// The "splitter" is the core routine that restores the invariant when a node
// overflows (holds 2t keys).  It promotes the median key to the parent and
// partitions the remaining keys into two balanced children.
// ---------------------------------------------------------------------------

// A single B-Tree node.  `keys` holds the sorted keys; `children` holds the
// child pointers.  For a leaf, `children` is empty.  For an internal node with
// k keys, `children` has k+1 entries.
struct BTreeNode {
    std::vector<int> keys;
    std::vector<BTreeNode*> children;
    bool isLeaf;

    BTreeNode() : isLeaf(true) {};
    explicit BTreeNode(bool leaf) : isLeaf(leaf) {}
};

// Result of splitting an overflowing node.
struct SplitResult {
    BTreeNode* left;    // new left child (or the original node, reused)
    BTreeNode* right;   // new right child
    int medianKey;      // key promoted to the parent
};

// The B-Tree itself.
class BTree {
public:
    explicit BTree(int minDegree);
    ~BTree();

    // Non-copyable, non-movable (raw pointers).
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    void insert(int key);
    bool search(int key) const;
    void remove(int key);

    // Expose the splitter for direct unit testing.
    SplitResult splitNode(BTreeNode* node);

    // Invariant checker: returns true iff the tree satisfies all B-Tree rules.
    bool verifyInvariants() const;

    // Depth of the tree (root = depth 1; empty tree = depth 0).
    int depth() const;

    // Total number of keys stored.
    size_t size() const;

    // In-order traversal into `out`.
    void inorder(std::vector<int>& out) const;

    int minDegree() const { return t_; };
    BTreeNode* root() const { return root_; }

private:
    int t_;
    BTreeNode* root_;
    size_t keyCount_;

    void insertNonFull(BTreeNode* node, int key);
    void removeKey(BTreeNode* node, int key);
    void removeFromNode(BTreeNode* node, int idx);
    void borrowFromPrev(BTreeNode* node, int idx);
    void borrowFromNext(BTreeNode* node, int idx);
    void mergeChildren(BTreeNode* node, int idx);
    int findKey(BTreeNode* node, int key) const;
    size_t countKeys(BTreeNode* node) const;
    bool checkNode(BTreeNode* node, int expectedDepth, int currentDepth) const;
};

} // namespace btree
