#include <utility>

#pragma once
#include <vector>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <cassert>

struct FibNode {
    int key;
    int vertex;
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;
    int degree;
    bool mark;
    FibNode(int k, int v)
        : key(k), vertex(v), parent(nullptr), child(nullptr),
          left(this), right(this), degree(0), mark(false) {};
};

class FibonacciHeap {
public:
    FibonacciHeap();
    ~FibonacciHeap();
    void insert(int key, int vertex);
    FibNode* extract_min();
    void decrease_key(FibNode* node, int new_key);
    bool empty() const;
private:
    FibNode* min;
    int n;
    std::vector<FibNode*> all_nodes;
    void consolidate();
    void link(FibNode* y, FibNode* x);
    void cut(FibNode* x, FibNode* y);
    void cascading_cut(FibNode* y);
    void add_to_root_list(FibNode* node);
    void remove_from_root_list(FibNode* node);
};

std::vector<int> dijkstra(const std::vector<std::vector<std::pair<int,int>>>& graph, int source);
