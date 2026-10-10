#include "core.hpp"

namespace btree {

template<typename K>
void BTree<K>::insert(const K& k) {
    if (root->keys.size() == 2 * t - 1) {
        BTreeNode<K>* s = new BTreeNode<K>(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    insertNonFull(root, k);
}

template<typename K>
void BTree<K>::splitChild(BTreeNode<K>* parent, int idx) {
    BTreeNode<K>* y = parent->children[idx];
    BTreeNode<K>* z = new BTreeNode<K>(y->leaf);

    // Median key
    K mid = y->keys[t - 1];

    // Move keys to z
    z->keys.assign(y->keys.begin() + t, y->keys.end());
    y->keys.resize(t - 1);

    // Move children if not leaf
    if (!y->leaf) {
        z->children.assign(y->children.begin() + t, y->children.end());
        y->children.resize(t);
    }

    parent->children.insert(parent->children.begin() + idx + 1, z);
    parent->keys.insert(parent->keys.begin() + idx, mid);
}

template<typename K>
void BTree<K>::insertNonFull(BTreeNode<K>* node, const K& k) {
    int i = static_cast<int>(node->keys.size()) - 1;
    if (node->leaf) {
        node->keys.push_back(K());
        while (i >= 0 && k < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            --i;
        }
        node->keys[i + 1] = k;
    } else {
        while (i >= 0 && k < node->keys[i]) --i;
        ++i;
        if (node->children[i]->keys.size() == 2 * t - 1) {
            splitChild(node, i);
            if (k > node->keys[i]) ++i;
        }
        insertNonFull(node->children[i], k);
    }
}

template<typename K>
bool BTree<K>::search(const K& k) const {
    BTreeNode<K>* cur = root;
    while (true) {
        int i = 0;
        while (i < static_cast<int>(cur->keys.size()) && k > cur->keys[i]) ++i;
        if (i < static_cast<int>(cur->keys.size()) && k == cur->keys[i]) return true;
        if (cur->leaf) return false;
        cur = cur->children[i];
    }
}

// Explicit instantiation for int
template class BTree<int>;

} // namespace btree
