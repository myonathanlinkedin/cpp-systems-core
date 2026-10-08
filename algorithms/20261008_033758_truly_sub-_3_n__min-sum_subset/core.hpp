#pragma once
#include "types.hpp"
#include <vector>
namespace minsum {
    std::vector<int> subset_convolution_min_sum(const std::vector<int>& f, const std::vector<int>& g);
    std::vector<int> greedy_join_order(const std::vector<int>& subsets);
}
