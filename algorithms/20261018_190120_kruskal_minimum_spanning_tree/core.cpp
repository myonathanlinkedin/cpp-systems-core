#include <vector>

#include "core.hpp"
#include <algorithm>
#include <numeric>

namespace kruskal {

// ---------- DSU implementation ----------
DSU::DSU(std::size_t n)
    : parent_(n), rank_(n, 0), components_(n) {
    std::iota(parent_.begin(), parent_.end(), 0);
}

std::size_t DSU::find(std::size_t x) {
    if (parent_[x] != x) {
        parent_[x] = find(parent_[x]); // path compression
    }
    return parent_[x];
}

bool DSU::unite(std::size_t x, std::size_t y) {
    std::size_t xr = find(x);
    std::size_t yr = find(y);
    if (xr == yr) return false; // already in same set

    // union by rank
    if (rank_[xr] < rank_[yr]) {
        parent_[xr] = yr;
    } else if (rank_[xr] > rank_[yr]) {
        parent_[yr] = xr;
    } else {
        parent_[yr] = xr;
        ++rank_[xr];
    }
    --components_;
    return true;
}

std::size_t DSU::component_count() const {
    return components_;
}

// ---------- Kruskal implementation ----------
std::vector<Edge> kruskalMST(int n,
                             const std::vector<Edge>& edges,
                             long long& totalWeight) {
    totalWeight = 0;
    if (n <= 0) return {};

    // copy edges and sort by weight (non‑decreasing)
    std::vector<Edge> sorted = edges;
    std::sort(sorted.begin(), sorted.end());

    DSU dsu(static_cast<std::size_t>(n));
    std::vector<Edge> mst;
    mst.reserve(static_cast<std::size_t>(n - 1));

    for (const Edge& e : sorted) {
        if (dsu.unite(static_cast<std::size_t>(e.u),
                      static_cast<std::size_t>(e.v))) {
            mst.push_back(e);
            totalWeight += e.weight;
            // early exit when we already have a spanning tree for a component
            if (mst.size() == static_cast<std::size_t>(n - dsu.component_count())) {
                // continue to allow forest creation for disconnected graphs
                // (no early break)
                ;
            }
        }
    }
    return mst;
}

} // namespace kruskal
