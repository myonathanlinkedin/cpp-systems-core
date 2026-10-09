#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

template <typename T>
class AVLTree {
    struct Node {
        T key;
        Node* left;
        Node* right;
        int height;
        Node(const T& k) : key(k), left(nullptr), right(nullptr), height(1) {}
    };
    Node* root = nullptr;

    static int h(Node* n) { return n ? n->height : 0; }
    static int bf(Node* n) { return n ? h(n->left) - h(n->right) : 0; }
    static void upd(Node* n) { if (n) n->height = 1 + std::max(h(n->left), h(n->right)); }

    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        upd(y);
        upd(x);
        return x;
    }
    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        upd(x);
        upd(y);
        return y;
    }
    static Node* balance(Node* n) {
        upd(n);
        int balance = bf(n);
        if (balance > 1) {
            if (bf(n->left) < 0) n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (balance < -1) {
            if (bf(n->right) > 0) n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }
    static Node* insertRec(Node* node, const T& key) {
        if (!node) return new Node(key);
        if (key < node->key) node->left = insertRec(node->left, key);
        else if (key > node->key) node->right = insertRec(node->right, key);
        else return node; // duplicate ignored
        return balance(node);
    }
    static Node* minValueNode(Node* node) {
        Node* cur = node;
        while (cur && cur->left) cur = cur->left;
        return cur;
    }
    static Node* eraseRec(Node* node, const T& key) {
        if (!node) return nullptr;
        if (key < node->key) node->left = eraseRec(node->left, key);
        else if (key > node->key) node->right = eraseRec(node->right, key);
        else {
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }
            Node* succ = minValueNode(node->right);
            node->key = succ->key;
            node->right = eraseRec(node->right, succ->key);
        }
        return balance(node);
    }
    static bool containsRec(Node* node, const T& key) {
        if (!node) return false;
        if (key < node->key) return containsRec(node->left, key);
        if (key > node->key) return containsRec(node->right, key);
        return true;
    }
    static void inorderRec(Node* node, std::vector<T>& out) {
        if (!node) return;
        inorderRec(node->left, out);
        out.push_back(node->key);
        inorderRec(node->right, out);
    }
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    ~AVLTree() { destroy(root); }
    void insert(const T& key) { root = insertRec(root, key); }
    void erase(const T& key) { root = eraseRec(root, key); }
    bool contains(const T& key) const { return containsRec(root, key); }
    std::vector<T> inorder() const { std::vector<T> v; inorderRec(root, v); return v; }
    int height() const { return h(root); }
};

int main() {
    AVLTree<int> avl;
    std::vector<int> data = {10, 20, 30, 40, 50, 25};
    for (int v : data) avl.insert(v);
    assert(avl.contains(30));
    assert(!avl.contains(99));
    std::vector<int> sorted = avl.inorder();
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());
    assert(sorted == expected);
    assert(avl.height() <= 3); // AVL height bound for 6 nodes

    avl.erase(20);
    assert(!avl.contains(20));
    sorted = avl.inorder();
    expected.erase(std::remove(expected.begin(), expected.end(), 20), expected.end());
    assert(sorted == expected);

    // Stress test random insert/erase
    AVLTree<int> t;
    std::vector<int> vals;
    for (int i = 0; i < 1000; ++i) {
        int x = rand() % 5000;
        t.insert(x);
        vals.push_back(x);
    }
    std::sort(vals.begin(), vals.end());
    vals.erase(std::unique(vals.begin(), vals.end()), vals.end());
    assert(t.inorder() == vals);
    for (int i = 0; i < 500; ++i) {
        int x = vals[rand() % vals.size()];
        t.erase(x);
        vals.erase(std::remove(vals.begin(), vals.end(), x), vals.end());
        assert(t.contains(x) == false);
    }
    assert(t.inorder() == vals);
    std::cout << "All AVL tests passed.\n";
    return 0;
}
