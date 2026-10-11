#include <memory>

#include "types.hpp"

namespace btree {

template<typename Key>
BTree<Key>::BTree(int t) : t(t), root(std::make_unique<Node<Key>>(true)) {}

template<typename Key>
void BTree<Key>::insert(const Key& k) {
    Node<Key>* r = root.get();
    if (r->keys.size() == 2 * t - 1) {
        auto s = std::make_unique<Node<Key>>(false);
        s->children.push_back(std::move(root));
        splitChild(s.get(), 0);
        root = std::move(s);
        insertNonFull(root.get(), k);
    } else {
        insertNonFull(r, k);
    }
}

template<typename Key>
void BTree<Key>::splitChild(Node<Key>* x, int i) {
    Node<Key>* y = x->children[i].get();
    auto z = std::make_unique<Node<Key>>(y->leaf);
    Key mid = y->keys[t - 1];

    // Move keys from y to z
    z->keys.assign(y->keys.begin() + t, y->keys.end());
    y->keys.resize(t - 1);

    // Move children if not leaf
    if (!y->leaf) {
        for (int j = t; j < static_cast<int>(y->children.size()); ++j) {
            z->children.push_back(std::move(y->children[j]));
        }
        y->children.resize(t);
    }

    x->keys.insert(x->keys.begin() + i, mid);
    x->children.insert(x->children.begin() + i + 1, std::move(z));
}

template<typename Key>
void BTree<Key>::insertNonFull(Node<Key>* x, const Key& k) {
    int i = static_cast<int>(x->keys.size()) - 1;
    if (x->leaf) {
        while (i >= 0 && k < x->keys[i]) {
            --i;
        }
        x->keys.insert(x->keys.begin() + i + 1, k);
    } else {
        while (i >= 0 && k < x->keys[i]) {
            --i;
        }
        ++i;
        if (x->children[i]->keys.size() == 2 * t - 1) {
            splitChild(x, i);
            if (k > x->keys[i]) {
                ++i;
            }
        }
        insertNonFull(x->children[i].get(), k);
    }
}

template<typename Key>
bool BTree<Key>::search(const Key& k) const {
    return search(root.get(), k);
}

template<typename Key>
bool BTree<Key>::search(const Node<Key>* x, const Key& k) const {
    int i = 0;
    while (i < static_cast<int>(x->keys.size()) && k > x->keys[i]) {
        ++i;
    }
    if (i < static_cast<int>(x->keys.size()) && k == x->keys[i]) {
        return true;
    }
    if (x->leaf) {
        return false;
    }
    return search(x->children[i].get(), k);
}

// Explicit template instantiation for int
template class BTree<int>;

} // namespace btree
