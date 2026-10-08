#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <algorithm>

int main() {
    for (int n = 1; n <= 4; ++n) {
        size_t sz = 1u << n;
        std::vector<int> f(sz), g(sz);
        std::mt19937 rng(12345);
        std::uniform_int_distribution<int> dist(0, 10);
        for (size_t i = 0; i < sz; ++i) {
            f[i] = dist(rng);
            g[i] = dist(rng);
        }
        auto h = minsum::subset_convolution_min_sum(f, g);
        for (minsum::Mask mask = 0; mask < sz; ++mask) {
            int best = minsum::INF;
            for (minsum::Mask A = mask; ; A = (A - 1) & mask) {
                minsum::Mask B = mask ^ A;
                int cand = f[A] + g[B];
                if (cand < best) best = cand;
                if (A == 0) break;
            }
            assert(h[mask] == best);
        }
    }

    std::vector<int> subsets = {0b111, 0b001, 0b010, 0b100, 0b011};
    auto order = minsum::greedy_join_order(subsets);
    for (size_t i = 1; i < order.size(); ++i) {
        assert(__builtin_popcount(order[i-1]) <= __builtin_popcount(order[i]));
    }

    std::cout << "All tests passed.\n";
    return 0;
}
