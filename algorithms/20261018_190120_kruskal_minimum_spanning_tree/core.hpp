#pragma once
#include "types.hpp"
#include <vector>
#include <cstddef>

namespace kruskal {

// Disjoint Set Union with path compression and union by rank
class DSU {
public:
    explicit DSU(std::size_t n);
    std::size_t find(std::size_t x);
    bool unite(std::size_t x, std::size_t y);
    std::size_t component_count() const;

private:
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> rank_;
    std::size_t components_;
};

// Kruskal's algorithm
// n: number of vertices (assumed labelled 0..n-1)
// edges: list of all edges
// Returns the edges belonging to the Minimum Spanning Forest
// totalWeight is set to the sum of selected edge weights
std::vector<Edge> kruskalMST(int n,
                             const std::vector<Edge>& edges,
                             long long& totalWeight);

} // namespace kruskal
