#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "core.cpp"

using namespace iso;

bool approx_identity(const Matrix& M, double eps = 1e-4) {
    size_t n = M.size();
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            double target = (i == j) ? 1.0 : 0.0;
            if (std::abs(M[i][j] - target) > eps) return false;
        }
    return true;
}

void test_isotropic_rounding() {
    const size_t dim = 5;
    const size_t pts = 1000;
    auto data = random_normal_points(pts, dim);
    Matrix T = isotropic_rounding(data, 50, 1e-5);
    auto transformed = transform(data, T);
    Matrix cov = covariance(transformed);
    assert(approx_identity(cov, 5e-3));
    std::cout << "Test passed: isotropic rounding yields near‑identity covariance.\n";
}

int main() {
    test_isotropic_rounding();
    return 0;
}
