#include <vector>
#include <numeric>
#include <unordered_map>
#include <map>
#include <set>
#include <utility>
#include <cmath>

#include "core.hpp"

FibonacciHeap::FibonacciHeap() : min(nullptr), n(0) {}

FibonacciHeap::~FibonacciHeap() {
    for (FibNode* node : all_nodes) delete node;
}

void FibonacciHeap::add_to_root_list(FibNode* node) {
    if (!min) {
        min = node;
    } else {
        node->left = min;
        node->right = min->right;
        min->right->left = node;
        min->right = node;
        if (node->key < min->key) min = node;
    }
}

void FibonacciHeap::remove_from_root_list(FibNode* node) {
    if (node->right == node) {
        min = nullptr;
    } else {
        node->left->right = node->right;
        node->right->left = node->left;
        if (min == node) min = node->right;
    }
    node->left = node;
    node->right = node;
}

void FibonacciHeap::insert(int key, int vertex) {
    FibNode* node = new FibNode(key, vertex);
    all_nodes.push_back(node);
    add_to_root_list(node);
    ++n;
}

FibNode* FibonacciHeap::extract_min() {
    FibNode* z = min;
    if (!z) return nullptr;
    if (z->child) {
        std::vector<FibNode*> children;
        FibNode* c = z->child;
        do {
            children.push_back(c);
            c = c->right;
        } while (c != z->child);
        for (FibNode* child : children) {
            child->parent = nullptr;
            add_to_root_list(child);
        }
    }
    remove_from_root_list(z);
    --n;
    if (n == 0) {
        min = nullptr;
    } else {
        min = z->right;
        consolidate();
    }
    return z;
}

void FibonacciHeap::link(FibNode* y, FibNode* x) {
    remove_from_root_list(y);
    y->parent = x;
    if (!x->child) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    ++x->degree;
    y->mark = false;
}

void FibonacciHeap::cut(FibNode* x, FibNode* y) {
    if (x->right == x) {
        y->child = nullptr;
    } else {
        if (y->child == x) y->child = x->right;
        x->left->right = x->right;
        x->right->left = x->left;
    }
    --y->degree;
    add_to_root_list(x);
    x->parent = nullptr;
    x->mark = false;
}

void FibonacciHeap::cascading_cut(FibNode* y) {
    FibNode* z = y->parent;
    if (z) {
        if (!y->mark) {
            y->mark = true;
        } else {
            cut(y, z);
            cascading_cut(z);
        }
    }
}

void FibonacciHeap::decrease_key(FibNode* node, int new_key) {
    if (new_key > node->key) return;
    node->key = new_key;
    FibNode* y = node->parent;
    if (y && node->key < y->key) {
        cut(node, y);
        cascading_cut(y);
    }
    if (node->key < min->key) min = node;
}

bool FibonacciHeap::empty() const { return n == 0; }

void FibonacciHeap::consolidate() {
    int max_degree = static_cast<int>(std::log2(n)) + 1;
    std::vector<FibNode*> A(max_degree + 1, nullptr);
    std::vector<FibNode*> roots;
    FibNode* w = min;
    if (w) {
        do {
            roots.push_back(w);
            w = w->right;
        } while (w != min);
    }
    for (FibNode* w_node : roots) {
        FibNode* x = w_node;
        int d = x->degree;
        while (A[d]) {
            FibNode* y = A[d];
            if (x->key > y->key) std::swap(x, y);
            link(y, x);
            A[d] = nullptr;
            ++d;
        }
        A[d] = x;
    }
    min = nullptr;
    for (FibNode* node : A) {
        if (node) {
            node->left = node;
            node->right = node;
            if (!min) {
                min = node;
            } else {
                add_to_root_list(node);
                if (node->key < min->key) min = node;
            }
        }
    }
}

std::vector<int> dijkstra(const std::vector<std::vector<std::pair<int,int>>>& graph, int source) {
    const int INF = std::numeric_limits<int>::max() / 2;
    std::vector<int> dist(graph.size(), INF);
    FibonacciHeap heap;
    std::unordered_map<int, FibNode*> node_map;
    for (size_t i = 0; i < graph.size(); ++i) {
        int key = (static_cast<int>(i) == source) ? 0 : INF;
        heap.insert(key, static_cast<int>(i));
        node_map[static_cast<int>(i)] = heap.extract_min(); // temporarily extract to get pointer
        heap.insert(key, static_cast<int>(i)); // reinsert
    }
    // Rebuild node_map with actual nodes
    node_map.clear();
    for (size_t i = 0; i < graph.size(); ++i) {
        FibNode* node = heap.extract_min();
        node_map[static_cast<int>(i)] = node;
        heap.insert(node->key, node->vertex);
    }
    // Now heap contains all nodes again
    while (!heap.empty()) {
        FibNode* minNode = heap.extract_min();
        int u = minNode->vertex;
        int du = minNode->key;
        if (du == INF) break;
        dist[u] = du;
        for (const auto& edge : graph[u]) {
            int v = edge.first;
            int w = edge.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                heap.decrease_key(node_map[v], dist[v]);
            }
        }
    }
    return dist;
}
