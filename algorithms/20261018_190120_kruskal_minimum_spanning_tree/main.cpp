#include <iostream>
#include <vector>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

using namespace kruskal;

// Helper to verify that MST is a forest (no cycles) and connects each component
static bool is_forest_valid(int n,
                            const std::vector<Edge>& mst,
                            const std::vector<Edge>& allEdges) {
    DSU dsu(static_cast<std::size_t>(n));
    for (const Edge& e : mst) {
        // if an edge connects vertices already in the same set, a cycle exists
        if (!dsu.unite(static_cast<std::size_t>(e.u),
                       static_cast<std::size_t>(e.v))) {
            return false;
        }
    }
    // Count expected number of edges: n - number_of_components
    int expected = n - static_cast<int>(dsu.component_count());
    return static_cast<int>(mst.size()) == expected;
}

// Unit test 1: Simple triangle graph
static void test_triangle() {
    int n = 3;
    std::vector<Edge> edges = {
        {0, 1, 4},
        {1, 2, 1},
        {0, 2, 3}
    };
    long long total = 0;
    auto mst = kruskalMST(n, edges, total);
    // Expected MST weight = 1 + 3 = 4
    assert(total == 4);
    assert(mst.size() == 2);
    assert(is_forest_valid(n, mst, edges));
}

// Unit test 2: Disconnected graph (two components)
static void test_disconnected() {
    int n = 6;
    std::vector<Edge> edges = {
        // Component A
        {0, 1, 2},
        {1, 2, 3},
        {0, 2, 1},
        // Component B
        {3, 4, 5},
        {4, 5, 6},
        {3, 5, 4}
    };
    long long total = 0;
    auto mst = kruskalMST(n, edges, total);
    // Component A MST weight = 1 + 2 = 3
    // Component B MST weight = 4 + 5 = 9
    assert(total == 12);
    // Total edges = (3-1) + (3-1) = 4
    assert(mst.size() == 4);
    assert(is_forest_valid(n, mst, edges));
}

// Unit test 3: Larger dense graph
static void test_dense() {
    int n = 5;
    std::vector<Edge> edges;
    // Complete graph K5 with weight = i+j
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            edges.emplace_back(i, j, i + j);
        }
    }
    long long total = 0;
    auto mst = kruskalMST(n, edges, total);
    // For K5 with weight i+j, the MST weight is known to be 16
    // (edges: (0,1)=1, (0,2)=2, (0,3)=3, (0,4)=4)
    assert(total == 10); // Actually sum 1+2+3+4 = 10
    assert(mst.size() == static_cast<std::size_t>(n - 1));
    assert(is_forest_valid(n, mst, edges));
}

// Entry point running all tests
int main() {
    test_triangle();
    test_disconnected();
    test_dense();

    std::cout << "All Kruskal MST unit tests passed.\n";
    return 0;
}
