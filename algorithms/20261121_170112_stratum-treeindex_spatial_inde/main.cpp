#include <vector>
#include <cassert>
#include <algorithm>

#include <iostream>
#include "types.hpp"
#include "engine.cpp" // include implementation (single translation unit for this demo)

using namespace stratum;

int main() {
    // Basic sanity checks.
    Tree idx(4); // small maxEntries to force splits in tests.

    // Insert a set of boxes.
    idx.insert(1, Box({0,0}, {2,2}));
    idx.insert(2, Box({1,1}, {3,3}));
    idx.insert(3, Box({5,5}, {7,7}));
    idx.insert(4, Box({6,0}, {8,2}));
    idx.insert(5, Box({-3,-3}, {-1,-1}));
    idx.insert(6, Box({-2,4}, {0,6}));
    idx.insert(7, Box({3,3}, {4,4}));
    idx.insert(8, Box({2,5}, {5,8}));

    // Query overlapping a central region.
    {
        Box q({1,1}, {4,4});
        auto res = idx.query(q);
        std::vector<int> expected = {1,2,7};
        std::sort(res.begin(), res.end());
        std::sort(expected.begin(), expected.end());
        assert(res == expected);
    }

    // Query a region with no overlaps.
    {
        Box q({10,10}, {12,12});
        auto res = idx.query(q);
        assert(res.empty());
    }

    // Query exact match.
    {
        Box q({5,5}, {7,7});
        auto res = idx.query(q);
        assert(res.size() == 1 && res[0] == 3);
    }

    // Query that hits multiple disjoint boxes.
    {
        Box q({-4,-4}, {10,10});
        auto res = idx.query(q);
        std::vector<int> expected = {1,2,3,4,5,6,7,8};
        std::sort(res.begin(), res.end());
        std::sort(expected.begin(), expected.end());
        assert(res == expected);
    }

    // Edge case: query exactly on the border (should count as intersect).
    {
        Box q({2,2}, {5,5});
        auto res = idx.query(q);
        // Boxes 2 (1,1)-(3,3) and 7 (3,3)-(4,4) intersect at the border.
        std::vector<int> expected = {2,7,8};
        std::sort(res.begin(), res.end());
        std::sort(expected.begin(), expected.end());
        assert(res == expected);
    }

    std::cout << "All Stratum‑TreeIndex unit tests passed.\n";
    return 0;
}
