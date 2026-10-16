#include <vector>

#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>

static Matrix naiveTranspose(const Matrix& src) {
    size_t n = src.size();
    Matrix dst(n, std::vector<int>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            dst[j][i] = src[i][j];
    return dst;
}

static void testSmallMatrices() {
    // 1x1
    Matrix m1 = {{42}};
    Matrix t1 = cacheObliviousTranspose(m1);
    assert(t1 == naiveTranspose(m1));

    // 2x2
    Matrix m2 = {{1, 2}, {3, 4}};
    Matrix t2 = cacheObliviousTranspose(m2);
    assert(t2 == naiveTranspose(m2));

    // 3x3
    Matrix m3 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix t3 = cacheObliviousTranspose(m3);
    assert(t3 == naiveTranspose(m3));

    // 4x4
    Matrix m4 = {{1, 2, 3, 4},
                 {5, 6, 7, 8},
                 {9,10,11,12},
                 {13,14,15,16}};
    Matrix t4 = cacheObliviousTranspose(m4);
    assert(t4 == naiveTranspose(m4));
}

static void testRandomMatrices() {
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (size_t n = 1; n <= 16; ++n) {
        Matrix m(n, std::vector<int>(n));
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                m[i][j] = dist(rng);
        Matrix t = cacheObliviousTranspose(m);
        assert(t == naiveTranspose(m));
    }
}

static void testEdgeCases() {
    // Empty matrix
    Matrix empty;
    Matrix t_empty = cacheObliviousTranspose(empty);
    assert(t_empty.empty());

    // 1x1 with negative value
    Matrix m1 = {{-5}};
    Matrix t1 = cacheObliviousTranspose(m1);
    assert(t1 == naiveTranspose(m1));
}

int main() {
    testSmallMatrices();
    testRandomMatrices();
    testEdgeCases();
    std::cout << "All tests passed.\n";
    return 0;
}
