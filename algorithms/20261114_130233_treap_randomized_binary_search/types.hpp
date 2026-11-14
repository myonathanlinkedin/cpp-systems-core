#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <cassert>

// Simple struct to hold test data; can be extended if needed.
struct TestData {
    std::vector<int> values;
    std::vector<int> expected_inorder;
};
