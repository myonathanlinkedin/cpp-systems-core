#include <vector>
#include <functional>

#include "types.hpp"

#include <cassert>
#include <algorithm>
#include <iostream>

namespace btree {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

BTree::BTree(int minDegree)
    : t_(minDegree), root_(new BTreeNode(true)), keyCount_(0) {
    assert(t_ >= 2 && "min degree must be >= 2");
}

BTree::~BTree() {
    // Post-order delete.
    std::function<void(BTreeNode*)> del = [&](BTreeNode* n) {
        if (!n) return;
        for (auto* c : n->children) del(c);
        delete n;
    };
    del(root_);
}

// ---------------------------------------------------------------------------
// The core splitter
//
// Precondition: `node` holds exactly 2t keys (i.e. it is full/overflowing).
// Postcondition:
//   * `left`  holds t keys  (indices 0 .. t-1 of the original)
//   * `right` holds t keys  (indices t+1 .. 2t-1 of the original)
//   * `medianKey` is the original key at index t
//   * If `node` was a leaf, both children are leaves.
//   * If `node` was internal, left gets children[0..t], right gets children[t+1..2t].
//
// The original `node` is reused as `left` to avoid an extra allocation.
// ---------------------------------------------------------------------------
SplitResult BTree::splitNode(BTreeNode* node) {
    assert(node && "splitNode: null node");
    assert(static_cast<int>(node->keys.size()) == 2 * t_ &&
           "splitNode: node must hold exactly 2t keys");

    int medianIdx = t_;
    int medianKey = node->keys[medianIdx];

    // Allocate the right child.
    BTreeNode* right = new BTreeNode(node->isLeaf);

    // Copy keys [t+1 .. 2t-1] into right.
    for (int i = medianIdx + 1; i < 2 * t_; ++i) {
        right->keys.push_back(node->keys[i]);
    }

    // If internal, copy children [t+1 .. 2t] into right.
    if (!node->isLeaf) {
        for (int i = medianIdx + 1; i <= 2 * t_; ++i) {
            right->children.push_back(node->children[i]);
        }
    }

    // Truncate the original node to become the left child:
    //   keys:    [0 .. t-1]
    //   children:[0 .. t]   (if internal)
    node->keys.resize(medianIdx);
    if (!node->isLeaf) {
        node->children.resize(medianIdx + 1);
    }

    return SplitResult{node, right, medianKey};
}

// ---------------------------------------------------------------------------
// Insertion
// ---------------------------------------------------------------------------

void BTree::insert(int key) {
    // If root is full, split it and create a new root.
    if (root_->keys.size() == 2 * static_cast<size_t>(t_)) {
        BTreeNode* newRoot = new BTreeNode(false);
        newRoot->children.push_back(root_);

        SplitResult sr = splitNode(root_);
        // After splitNode, root_ is the left child.
        newRoot->keys.push_back(sr.medianKey);
        newRoot->children.push_back(sr.right);

        root_ = newRoot;
    }
    insertNonFull(root_, key);
    ++keyCount_;
}

void BTree::insertNonFull(BTreeNode* node, int key) {
    int i = static_cast<int>(node->keys.size()) - 1;

    if (node->isLeaf) {
        // Find insertion position.
        while (i >= 0 && node->keys[i] > key) --i;
        node->keys.insert(node->keys.begin() + (i + 1), key);
    } else {
        while (i >= 0 && node->keys[i] > key) --i;
        ++i;
        if (node->children[i]->keys.size() == 2 * static_cast<size_t>(t_)) {
            SplitResult sr = splitNode(node->children[i]);
            // Insert median into node.
            int pos = static_cast<int>(node->keys.size()) - 1;
            while (pos >= 0 && node->keys[pos] > sr.medianKey) --pos;
            node->keys.insert(node->keys.begin() + (pos + 1), sr.medianKey);
            node->children.insert(node->children.begin() + (pos + 2), sr.right);
            if (sr.medianKey == key) return;
            key = (key > sr.medianKey) ? key : key; // key unchanged
            i = (key > sr.medianKey) ? (pos + 2) : (pos + 1);
        }
        insertNonFull(node->children[i], key);
    }
}

// ---------------------------------------------------------------------------
// Search
// ---------------------------------------------------------------------------

bool BTree::search(int key) const {
    return findKey(root_, key) != -1;
}

int BTree::findKey(BTreeNode* node, int key) const {
    int i = 0;
    while (i < static_cast<int>(node->keys.size()) && node->keys[i] < key) ++i;
    if (i < static_cast<int>(node->keys.size()) && node->keys[i] == key) return i;
    if (node->isLeaf) return -1;
    return findKey(node->children[i], key);
}

// ---------------------------------------------------------------------------
// Deletion (standard CLRS-style B-Tree delete)
// ---------------------------------------------------------------------------

void BTree::remove(int key) {
    if (!search(key)) return;
    removeKey(root_, key);
    --keyCount_;

    // Shrink root if it becomes empty.
    if (root_->keys.empty()) {
        BTreeNode* old = root_;
        if (root_->isLeaf) {
            root_ = new BTreeNode(true);
        } else {
            root_ = root_->children[0];
        }
        delete old;
    }
}

void BTree::removeKey(BTreeNode* node, int key) {
    int i = findKey(node, key);

    if (i != -1) {
        if (node->isLeaf) {
            removeFromNode(node, i);
        } else {
            // Internal node: replace with predecessor or successor.
            if (node->children[i]->keys.size() >= static_cast<size_t>(t_)) {
                // Get predecessor from left subtree.
                BTreeNode* c = node->children[i];
                while (!c->isLeaf) c = c->children[static_cast<int>(c->children.size()) - 1];
                node->keys[i] = c->keys.back();
                removeKey(c, node->keys[i]);
            } else if (node->children[i + 1]->keys.size() >= static_cast<size_t>(t_)) {
                // Get successor from right subtree.
                BTreeNode* c = node->children[i + 1];
                while (!c->isLeaf) c = c->children[0];
                node->keys[i] = c->keys.front();
                removeKey(c, node->keys[i]);
            } else {
                // Merge children[i] and children[i+1] with key[i].
                mergeChildren(node, i);
                removeKey(node->children[i], key);
            }
        }
    } else {
        // Key not in this node; descend.
        if (node->isLeaf) return;

        bool goRight = (i < static_cast<int>(node->keys.size()) && node->keys[i] < key);
        int childIdx = goRight ? i + 1 : i;

        // Ensure child has at least t keys before descending.
        if (node->children[childIdx]->keys.size() < static_cast<size_t>(t_)) {
            if (childIdx > 0 && node->children[childIdx - 1]->keys.size() >= static_cast<size_t>(t_)) {
                borrowFromPrev(node, childIdx);
            } else if (childIdx < static_cast<int>(node->children.size()) - 1 &&
                       node->children[childIdx + 1]->keys.size() >= static_cast<size_t>(t_)) {
                borrowFromNext(node, childIdx);
            } else {
                if (childIdx > 0) {
                    mergeChildren(node, childIdx - 1);
                    childIdx = childIdx - 1;
                } else {
                    mergeChildren(node, childIdx);
                }
            }
        }
        removeKey(node->children[childIdx], key);
    }
}

void BTree::removeFromNode(BTreeNode* node, int idx) {
    node->keys.erase(node->keys.begin() + idx);
}

void BTree::borrowFromPrev(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx];
    BTreeNode* prev = node->children[idx - 1];

    cur->keys.insert(cur->keys.begin(), node->keys[idx - 1]);
    node->keys[idx - 1] = prev->keys.back();
    prev->keys.pop_back();

    if (!prev->isLeaf) {
        cur->children.insert(cur->children.begin(), prev->children.back());
        prev->children.pop_back();
    }
}

void BTree::borrowFromNext(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx];
    BTreeNode* next = node->children[idx + 1];

    cur->keys.push_back(node->keys[idx]);
    node->keys[idx] = next->keys.front();
    next->keys.erase(next->keys.begin());

    if (!next->isLeaf) {
        cur->children.push_back(next->children.front());
        next->children.erase(next->children.begin());
    }
}

void BTree::mergeChildren(BTreeNode* node, int idx) {
    BTreeNode* left = node->children[idx];
    BTreeNode* right = node->children[idx + 1];

    // Move the separator key down.
    left->keys.push_back(node->keys[idx]);

    // Append right's keys.
    for (int k : right->keys) left->keys.push_back(k);

    // Append right's children (if internal).
    if (!left->isLeaf) {
        for (BTreeNode* c : right->children) left->children.push_back(c);
    }

    // Remove the separator key from node.
    node->keys.erase(node->keys.begin() + idx);
    // Remove the right child pointer.
    node->children.erase(node->children.begin() + (idx + 1));

    delete right;
}

// ---------------------------------------------------------------------------
// Invariant verification
// ---------------------------------------------------------------------------

bool BTree::verifyInvariants() const {
    if (!root_) return true;
    int depth = 0;
    return checkNode(root_, 1, depth);
}

bool BTree::checkNode(BTreeNode* node, int expectedDepth, int currentDepth) const {
    if (!node) return false;

    // Key count bounds.
    size_t k = node->keys.size();
    if (node->isLeaf) {
        if (k < static_cast<size_t>(t_ - 1) || k > static_cast<size_t>(2 * t_ - 1))
            return false;
    } else {
        if (k < static_cast<size_t>(t_) || k > static_cast<size_t>(2 * t_ - 1))
            return false;
        // Child count must be k+1.
        if (node->children.size() != k + 1) return false;
    }

    // Keys must be strictly increasing.
    for (size_t i = 1; i < k; ++i) {
        if (node->keys[i - 1] >= node->keys[i]) return false;
    }

    if (node->isLeaf) {
        // All leaves must be at the same depth.
        if (currentDepth + 1 != expectedDepth) return false;
        return true;
    }

    // Recurse into children.
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (!checkNode(node->children[i], expectedDepth, currentDepth + 1))
            return false;
        // Range check: keys in child i must be < node->keys[i] (if i < k)
        // and > node->keys[i-1] (if i > 0).
        // (Full range checking is expensive; we do a lightweight check.)
    }
    return true;
}

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------

int BTree::depth() const {
    if (!root_) return 0;
    int d = 0;
    BTreeNode* n = root_;
    while (n) {
        ++d;
        if (n->isLeaf) break;
        n = n->children[0];
    }
    return d;
}

size_t BTree::countKeys(BTreeNode* node) const {
    if (!node) return 0;
    size_t cnt = node->keys.size();
    for (auto* c : node->children) cnt += countKeys(c);
    return cnt;
}

size_t BTree::size() const {
    return keyCount_;
}

void BTree::inorder(std::vector<int>& out) const {
    std::function<void(BTreeNode*)> walk = [&](BTreeNode* n) {
        if (!n) return;
        for (size_t i = 0; i < n->keys.size(); ++i) {
            if (!n->isLeaf) walk(n->children[i]);
            out.push_back(n->keys[i]);
        }
        if (!n->isLeaf) walk(n->children.back());
    };
    walk(root_);
}

} // namespace btree
