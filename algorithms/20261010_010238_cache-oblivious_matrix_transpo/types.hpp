#pragma once
#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace lsm {
using Matrix = std::vector<std::vector<int>>;

inline Matrix generate_random_matrix(std::size_t rows, std::size_t cols, int min_val = 0, int max_val = 100) {
    Matrix mat(rows, std::vector<int>(cols));
    std::mt19937 rng(static_cast<unsigned>(std::random_device{}()));
    std::uniform_int_distribution<int> dist(min_val, max_val);
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            mat[i][j] = dist(rng);
        }
    }
    return mat;
}

inline Matrix naive_transpose(const Matrix& src) {
    std::size_t rows = src.size();
    std::size_t cols = rows > 0 ? src[0].size() : 0;
    Matrix dst(cols, std::vector<int>(rows));
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            dst[j][i] = src[i][j];
        }
    }
    return dst;
}

inline bool matrices_equal(const Matrix& a, const Matrix& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size()) return false;
        for (std::size_t j = 0; j < a[i].size(); ++j) {
            if (a[i][j] != b[i][j]) return false;
        }
    }
    return true;
}
} // namespace lsm
