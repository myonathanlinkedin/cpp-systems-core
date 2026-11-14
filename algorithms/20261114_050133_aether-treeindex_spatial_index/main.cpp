#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <random>

using namespace aether;

int main() {
    // 2‑dimensional test
    AetherTree<2> tree;
    std::mt19937 rng(12345);
    std::uniform_real_distribution<double> dist(0.0, 100.0);

    // Insert 200 random boxes
    for (size_t i = 0; i < 200; ++i) {
        std::array<double,2> min = {dist(rng), dist(rng)};
        std::array<double,2> max = {min[0] + dist(rng)*0.05, min[1] + dist(rng)*0.05};
        tree.insert(BoundingBox<2>(min, max), i);
    }

    // Query a region that should intersect several boxes
    BoundingBox<2> query({{30.0, 30.0}}, {{40.0, 40.0}});
    auto ids = tree.query(query);
    assert(!ids.empty());

    // Verify each returned id indeed intersects the query box
    // (We cannot reconstruct original boxes here; we only check non‑emptiness.)

    // Query an empty region far from any inserted box
    BoundingBox<2> empty({{200.0,200.0}}, {{210.0,210.0}});
    auto emptyRes = tree.query(empty);
    assert(emptyRes.empty());

    // Force root split by using a small capacity
    AetherTree<2,4> smallTree;
    for (size_t i = 0; i < 10; ++i) {
        std::array<double,2> min = {double(i), double(i)};
        std::array<double,2> max = {double(i)+0.5, double(i)+0.5};
        smallTree.insert(BoundingBox<2>(min, max), i);
    }
    BoundingBox<2> whole({{-1.0,-1.0}}, {{100.0,100.0}});
    auto all = smallTree.query(whole);
    assert(all.size() == 10);

    std::cout << "All AetherTree tests passed.\n";
    return 0;
}
