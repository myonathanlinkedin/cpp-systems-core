#include <vector>
#include <cmath>

#include "core.hpp"
#include "types.hpp"
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>

using namespace cg;

void test_small() {
    Matrix A = {{4.0, 1.0},
                {1.0, 3.0}};
    Vector b = {1.0, 2.0};
    Vector x0; // empty -> zero init
    Vector x = conjugateGradient(A, b, x0, 1e-10, 100);
    Vector expected = {0.09090909090909091, 0.6363636363636364};
    for (size_t i = 0; i < x.size(); ++i) {
        assert(std::abs(x[i] - expected[i]) < 1e-8);
    }
}

Matrix generateSPD(std::size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    Matrix M(n, std::vector<double>(n));
    for (auto& row : M)
        for (auto& val : row) val = dist(rng);
    Matrix A(n, std::vector<double>(n, 0.0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < n; ++k)
                A[i][j] += M[k][i] * M[k][j];
    for (size_t i = 0; i < n; ++i) A[i][i] += static_cast<double>(n);
    return A;
}

void test_random_spd() {
    const std::size_t n = 10;
    Matrix A = generateSPD(n);
    Vector x_true(n);
    for (size_t i = 0; i < n; ++i) x_true[i] = static_cast<double>(i + 1);
    Vector b = matVec(A, x_true);
    Vector x0; // zero init
    Vector x = conjugateGradient(A, b, x0, 1e-8, 500);
    Vector r = b;
    Vector Ax = matVec(A, x);
    for (size_t i = 0; i < n; ++i) r[i] -= Ax[i];
    assert(norm(r) / norm(b) < 1e-6);
    for (size_t i = 0; i < n; ++i) {
        assert(std::abs(x[i] - x_true[i]) < 1e-5);
    }
}

int main() {
    test_small();
    test_random_spd();
    std::cout << "All CG tests passed.\n";
    return 0;
}
