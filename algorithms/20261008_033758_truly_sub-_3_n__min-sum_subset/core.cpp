#include <vector>

#include "core.hpp"
#include <algorithm>
namespace minsum {
    std::vector<int> subset_convolution_min_sum(const std::vector<int>& f, const std::vector<int>& g) {
        size_t n = 0;
        while ((1u << n) < f.size()) ++n;
        std::vector<int> h(1u << n, INF);
        for (Mask mask = 0; mask < (1u << n); ++mask) {
            int best = INF;
            for (Mask A = mask; ; A = (A - 1) & mask) {
                Mask B = mask ^ A;
                int cand = f[A] + g[B];
                if (cand < best) best = cand;
                if (A == 0) break;
            }
            h[mask] = best;
        }
        return h;
    }

    std::vector<int> greedy_join_order(const std::vector<int>& subsets) {
        std::vector<int> order = subsets;
        std::sort(order.begin(), order.end(),
                  [](int a, int b){ return __builtin_popcount(a) < __builtin_popcount(b); });
        return order;
    }
}
