#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include "core.cpp"

using namespace cache_oblivious;

// Helper: naive transpose for verification.
template <typename T>
void naive_transpose(const std::vector<T>& src,
                     std::vector<T>& dst,
                     std::size_t rows,
                     std::size_t cols) {
    dst.assign(cols * rows, T{});
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            dst[j * rows + i] = src[i * cols + j];
        }
    }
}

// Helper: generate a matrix filled with sequential values.
template <typename T>
std::vector<T> generate_sequential(std::size_t rows, std::size_t cols) {
    std::vector<T> m(rows * cols);
    for (std::size_t i = 0; i < rows * cols; ++i) {
        m[i] = static_cast<T>(i);
    }
    return m;
}

// Helper: generate a matrix with random values.
template <typename T>
std::vector<T> generate_random(std::size_t rows, std::size_t cols, T min = T{0}, T max = T{100}) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<T> dist(min, max);
    std::vector<T> m(rows * cols);
    for (auto& v : m) v = dist(rng);
    return m;
}

// Unit tests.
void test_square_small() {
    const std::size_t N = 8;
    auto src = generate_sequential<int>(N, N);
    std::vector<int> dst, expected;
    Transposer<int>::transpose(src, dst, N, N);
    naive_transpose(src, expected, N, N);
    assert(dst == expected);
}

void test_rectangular_wide() {
    const std::size_t rows = 4, cols = 12;
    auto src = generate_random<int>(rows, cols);
    std::vector<int> dst, expected;
    Transposer<int>::transpose(src, dst, rows, cols);
    naive_transpose(src, expected, rows, cols);
    assert(dst == expected);
}

void test_rectangular_tall() {
    const std::size_t rows = 15, cols = 5;
    auto src = generate_random<int>(rows, cols);
    std::vector<int> dst, expected;
    Transposer<int>::transpose(src, dst, rows, cols);
    naive_transpose(src, expected, rows, cols);
    assert(dst == expected);
}

void test_double_transpose_identity() {
    const std::size_t rows = 23, cols = 31;
    auto src = generate_random<int>(rows, cols);
    std::vector<int> tmp, dst;
    Transposer<int>::transpose(src, tmp, rows, cols);
    Transposer<int>::transpose(tmp, dst, cols, rows); // note swapped dimensions
    assert(dst == src);
}

void test_large_random() {
    const std::size_t rows = 256, cols = 128;
    auto src = generate_random<int>(rows, cols);
    std::vector<int> dst, expected;
    Transposer<int>::transpose(src, dst, rows, cols);
    naive_transpose(src, expected, rows, cols);
    assert(dst == expected);
}

int main() {
    test_square_small();
    test_rectangular_wide();
    test_rectangular_tall();
    test_double_transpose_identity();
    test_large_random();

    std::cout << "All cache‑oblivious matrix transposition tests passed.\n";
    return 0;
}
