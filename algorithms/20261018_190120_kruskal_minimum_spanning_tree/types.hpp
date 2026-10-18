#pragma once
#include <vector>
#include <algorithm>
#include <cstddef>

namespace kruskal {

struct Edge {
    int u;          // one endpoint
    int v;          // other endpoint
    int weight;     // edge weight

    Edge() = default;
    Edge(int u_, int v_, int w_) : u(u_), v(v_), weight(w_) {};

    // needed for std::sort
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

} // namespace kruskal
