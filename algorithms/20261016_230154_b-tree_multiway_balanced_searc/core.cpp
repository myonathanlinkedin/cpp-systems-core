#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

struct BTreeNode {
    int t; // Minimum degree
    bool leaf;
    std::vector<int> keys;
    std::vector<std::unique_ptr<BTreeNode>> children;

    BTreeNode(int t, bool leaf) : t(t), leaf(leaf) {};
};

void splitChild(BTreeNode* parent, int index, BTreeNode* child) {
    int t = child->t;
    auto newNode = std::make_unique<BTreeNode>(t, child->leaf);

    // Move the last t-1 keys of child to newNode
    newNode->keys.assign(child->keys.begin() + t, child->keys.end());
    child->keys.resize(t - 1);

    // If child is not a leaf, move the last t children to newNode
    if (!child->leaf) {
        newNode->children.assign(
            std::make_move_iterator(child->children.begin() + t),
            std::make_move_iterator(child->children.end()));
        child->children.resize(t);
    }

    // Insert newNode into parent
    parent->children.insert(parent->children.begin() + index + 1, std::move(newNode));

    // Promote the middle key to parent
    parent->keys.insert(parent->keys.begin() + index, child->keys.back());
    child->keys.pop_back();
}

void insertNonFull(BTreeNode* node, int k) {
    int i = static_cast<int>(node->keys.size()) - 1;
    if (node->leaf) {
        node->keys.push_back(0);
        while (i >= 0 && node->keys[i] > k) {
            node->keys[i + 1] = node->keys[i];
            --i;
        }
        node->keys[i + 1] = k;
    } else {
        while (i >= 0 && node->keys[i] > k) --i;
        ++i;
        if (node->children[i]->keys.size() == 2 * node->t - 1) {
            splitChild(node, i, node->children[i].get());
            if (node->keys[i] < k) ++i;
        }
        insertNonFull(node->children[i].get(), k);
    }
}

struct BTree {
    int t;
    std::unique_ptr<BTreeNode> root;

    explicit BTree(int t) : t(t), root(std::make_unique<BTreeNode>(t, true)) {};

    void insert(int k) {
        if (root->keys.size() == 2 * t - 1) {
            auto s = std::make_unique<BTreeNode>(t, false);
            s->children.push_back(std::move(root));
            splitChild(s.get(), 0, s->children[0].get());
            root = std::move(s);
        }
        insertNonFull(root.get(), k);
    }
};
