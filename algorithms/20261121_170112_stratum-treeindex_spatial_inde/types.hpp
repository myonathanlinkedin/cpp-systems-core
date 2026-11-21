#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

namespace stratum {

// Fixed dimensionality for simplicity; can be templated if needed.
constexpr std::size_t DIM = 2;

// Axis-aligned bounding box in DIM dimensions.
struct Box {
    std::vector<double> min; // size DIM
    std::vector<double> max; // size DIM

    Box() : min(DIM, 0.0), max(DIM, 0.0) {};
    Box(const std::vector<double>& mn, const std::vector<double>& mx) : min(mn), max(mx) {
        assert(min.size() == DIM && max.size() == DIM);
        for (std::size_t i = 0; i < DIM; ++i) {
            assert(min[i] <= max[i]);
        }
    }
};

// Helper functions for Box.
inline bool intersects(const Box& a, const Box& b) {
    for (std::size_t i = 0; i < DIM; ++i) {
        if (a.max[i] < b.min[i] || b.max[i] < a.min[i]) return false;
    }
    return true;
}

inline Box combine(const Box& a, const Box& b) {
    Box res;
    for (std::size_t i = 0; i < DIM; ++i) {
        res.min[i] = std::min(a.min[i], b.min[i]);
        res.max[i] = std::max(a.max[i], b.max[i]);
    }
    return res;
}

// Hyper‑volume (area in 2‑D) of a box.
inline double volume(const Box& b) {
    double vol = 1.0;
    for (std::size_t i = 0; i < DIM; ++i) {
        vol *= (b.max[i] - b.min[i]);
    }
    return vol;
}

// Entry stored in leaf nodes.
struct Entry {
    int id;   // user supplied identifier
    Box box;  // bounding box of the entry

    Entry() = default;
    Entry(int i, const Box& b) : id(i), box(b) {};
};

// Forward declaration.
struct Node;

// Node of the Stratum‑TreeIndex.
struct Node {
    bool leaf;                                 // true if leaf node
    Box mbr;                                   // minimum bounding rectangle covering children/entries
    std::vector<Entry> entries;                // valid only if leaf == true
    std::vector<std::unique_ptr<Node>> childs; // valid only if leaf == false

    explicit Node(bool isLeaf);
    void recalcMBR(); // recompute mbr from children or entries
};

struct Tree {
    // R‑tree style parameters.
    const std::size_t maxEntries; // M
    const std::size_t minEntries; // m (usually M/2)

    std::unique_ptr<Node> root;

    Tree(std::size_t maxE = 8);
    void insert(int id, const Box& box);
    std::vector<int> query(const Box& q) const;

private:
    void insertRecursive(Node* node, const Entry& e);
    void splitNode(Node* node);
    void queryRecursive(const Node* node, const Box& q, std::vector<int>& out) const;
};

} // namespace stratum
