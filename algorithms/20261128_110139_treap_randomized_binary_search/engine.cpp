#include <vector>
#include <memory>
#include <random>
#include <utility>

#include "types.hpp"

namespace treap {

template <typename K, typename V>
class Treap {
public:
    // Public interface
    Treap() : root(nullptr), rng(std::random_device{}()) {}

    // Insert or update a key/value pair
    void insert(const K& key, const V& value) {
        uint32_t prio = rng();
        root = insert(std::move(root), std::make_unique<Node<K, V>>(key, value, prio));
    }

    // Erase a key; returns true if key existed
    bool erase(const K& key) {
        bool erased = false;
        root = erase(std::move(root), key, erased);
        return erased;
    }

    // Find value associated with key; returns pointer or nullptr
    const V* find(const K& key) const {
        const Node<K, V>* cur = root.get();
        while (cur) {
            if (key < cur->key) cur = cur->left.get();
            else if (key > cur->key) cur = cur->right.get();
            else return &(cur->value);
        }
        return nullptr;
    }

    // Return keys in sorted order
    std::vector<K> inorder() const {
        std::vector<K> out;
        inorder(root.get(), out);
        return out;
    }

    // Validate BST and heap properties (used in tests)
    bool validate() const {
        return validateBST(root.get(), nullptr, nullptr) && validateHeap(root.get());
    }

private:
    std::unique_ptr<Node<K, V>> root;
    std::mt19937 rng;

    // Split treap rooted at cur into left (< key) and right (>= key)
    static std::pair<std::unique_ptr<Node<K, V>>, std::unique_ptr<Node<K, V>>>
    split(std::unique_ptr<Node<K, V>> cur, const K& key) {
        if (!cur) return {nullptr, nullptr};
        if (key <= cur->key) {
            auto leftSplit = split(std::move(cur->left), key);
            cur->left = std::move(leftSplit.second);
            return {std::move(leftSplit.first), std::move(cur)};
        } else {
            auto rightSplit = split(std::move(cur->right), key);
            cur->right = std::move(rightSplit.first);
            return {std::move(cur), std::move(rightSplit.second)};
        }
    }

    // Merge two treaps where all keys in left < all keys in right
    static std::unique_ptr<Node<K, V>> merge(std::unique_ptr<Node<K, V>> left,
                                             std::unique_ptr<Node<K, V>> right) {
        if (!left) return right;
        if (!right) return left;
        if (left->priority < right->priority) {
            left->right = merge(std::move(left->right), std::move(right));
            return left;
        } else {
            right->left = merge(std::move(left), std::move(right->left));
            return right;
        }
    }

    // Insert node into treap rooted at cur
    static std::unique_ptr<Node<K, V>> insert(std::unique_ptr<Node<K, V>> cur,
                                              std::unique_ptr<Node<K, V>> node) {
        if (!cur) return node;
        if (node->priority < cur->priority) {
            // Node becomes new root of this subtree
            auto splitted = split(std::move(cur), node->key);
            node->left = std::move(splitted.first);
            node->right = std::move(splitted.second);
            return node;
        } else if (node->key < cur->key) {
            cur->left = insert(std::move(cur->left), std::move(node));
        } else if (node->key > cur->key) {
            cur->right = insert(std::move(cur->right), std::move(node));
        } else {
            // Duplicate key: replace value, discard node
            cur->value = node->value;
        }
        return cur;
    }

    // Erase key from treap rooted at cur
    static std::unique_ptr<Node<K, V>> erase(std::unique_ptr<Node<K, V>> cur,
                                             const K& key, bool& erased) {
        if (!cur) return nullptr;
        if (key < cur->key) {
            cur->left = erase(std::move(cur->left), key, erased);
        } else if (key > cur->key) {
            cur->right = erase(std::move(cur->right), key, erased);
        } else {
            // Found node to delete
            erased = true;
            return merge(std::move(cur->left), std::move(cur->right));
        }
        return cur;
    }

    // Inorder traversal helper
    static void inorder(const Node<K, V>* cur, std::vector<K>& out) {
        if (!cur) return;
        inorder(cur->left.get(), out);
        out.push_back(cur->key);
        inorder(cur->right.get(), out);
    }

    // BST validation: all keys in subtree must be within (min, max)
    static bool validateBST(const Node<K, V>* cur,
                            const K* minKey,
                            const K* maxKey) {
        if (!cur) return true;
        if ((minKey && !(cur->key > *minKey)) ||
            (maxKey && !(cur->key < *maxKey))) {
            return false;
        }
        return validateBST(cur->left.get(), minKey, &(cur->key)) &&
               validateBST(cur->right.get(), &(cur->key), maxKey);
    }

    // Heap validation: parent priority <= children priority
    static bool validateHeap(const Node<K, V>* cur) {
        if (!cur) return true;
        if (cur->left && cur->left->priority < cur->priority) return false;
        if (cur->right && cur->right->priority < cur->priority) return false;
        return validateHeap(cur->left.get()) && validateHeap(cur->right.get());
    }
};

} // namespace treap

// Explicit template instantiation for int keys/values (used in tests)
template class treap::Treap<int, int>;
