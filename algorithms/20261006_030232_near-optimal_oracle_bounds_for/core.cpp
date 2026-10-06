#include <vector>
#include <random>
#include <cmath>
#include <cassert>
#include <iostream>

namespace iso {

// Simple matrix type
using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

// Create an n×n zero matrix
inline Matrix zeros(size_t n) {
    return Matrix(n, Vector(n, 0.0));
}

// Identity matrix
inline Matrix identity(size_t n) {
    Matrix I = zeros(n);
    for (size_t i = 0; i < n; ++i) I[i][i] = 1.0;
    return I;
}

// Matrix‑vector product
inline Vector mat_vec_mul(const Matrix& A, const Vector& x) {
    size_t n = A.size();
    assert(x.size() == n);
    Vector y(n, 0.0);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            y[i] += A[i][j] * x[j];
    return y;
}

// Matrix‑matrix product
inline Matrix mat_mul(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    assert(B.size() == n && B[0].size() == n);
    Matrix C = zeros(n);
    for (size_t i = 0; i < n; ++i)
        for (size_t k = 0; k < n; ++k)
            for (size_t j = 0; j < n; ++j)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

// Transpose
inline Matrix transpose(const Matrix& A) {
    size_t n = A.size();
    Matrix At = zeros(n);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            At[j][i] = A[i][j];
    return At;
}

// Gaussian elimination for symmetric positive definite matrices
inline Matrix invert(const Matrix& A) {
    size_t n = A.size();
    Matrix aug = A;
    Matrix I = identity(n);
    // Forward elimination
    for (size_t i = 0; i < n; ++i) {
        // Pivot
        double piv = aug[i][i];
        assert(std::abs(piv) > 1e-12);
        for (size_t j = 0; j < n; ++j) {
            aug[i][j] /= piv;
            I[i][j]   /= piv;
        }
        // Eliminate other rows
        for (size_t k = 0; k < n; ++k) {
            if (k == i) continue;
            double factor = aug[k][i];
            for (size_t j = 0; j < n; ++j) {
                aug[k][j] -= factor * aug[i][j];
                I[k][j]   -= factor * I[i][j];
            }
        }
    }
    return I;
}

// Compute mean vector of points
inline Vector mean(const std::vector<Vector>& pts) {
    size_t n = pts[0].size();
    Vector mu(n, 0.0);
    for (const auto& p : pts)
        for (size_t i = 0; i < n; ++i)
            mu[i] += p[i];
    double inv = 1.0 / pts.size();
    for (double& v : mu) v *= inv;
    return mu;
}

// Compute covariance matrix (unbiased)
inline Matrix covariance(const std::vector<Vector>& pts) {
    size_t n = pts[0].size();
    Vector mu = mean(pts);
    Matrix cov = zeros(n);
    for (const auto& p : pts) {
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                cov[i][j] += (p[i] - mu[i]) * (p[j] - mu[j]);
    }
    double scale = 1.0 / (pts.size() - 1);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            cov[i][j] *= scale;
    return cov;
}

// Apply linear transformation to all points
inline std::vector<Vector> transform(const std::vector<Vector>& pts, const Matrix& T) {
    std::vector<Vector> out;
    out.reserve(pts.size());
    for (const auto& p : pts)
        out.emplace_back(mat_vec_mul(T, p));
    return out;
}

// Near‑optimal isotropic rounding using iterative covariance scaling
inline Matrix isotropic_rounding(const std::vector<Vector>& points,
                                 int max_iter = 100,
                                 double tol = 1e-6) {
    assert(!points.empty());
    size_t n = points[0].size();
    Matrix T = identity(n);
    std::vector<Vector> cur = points;

    for (int it = 0; it < max_iter; ++it) {
        Matrix cov = covariance(cur);
        // Compute matrix sqrt of inverse covariance via eigen‑free method:
        // Use Cholesky‑like approach: invert then take matrix square root via
        // Newton–Schulz iteration for matrix inverse square root.
        Matrix inv_cov = invert(cov);
        // Initial guess: identity
        Matrix Y = identity(n);
        Matrix Z = inv_cov;
        // Newton–Schulz: Y_{k+1} = 0.5 * Y_k * (3I - Z_k * Y_k)
        //                Z_{k+1} = 0.5 * (3I - Z_k * Y_k) * Z_k
        for (int k = 0; k < 5; ++k) {
            Matrix YZ = mat_mul(Y, Z);
            Matrix threeI = identity(n);
            for (size_t i = 0; i < n; ++i) threeI[i][i] *= 3.0;
            Matrix M = zeros(n);
            for (size_t i = 0; i < n; ++i)
                for (size_t j = 0; j < n; ++j)
                    M[i][j] = threeI[i][j] - YZ[i][j];
            // Update Y and Z
            Matrix Ynew = zeros(n), Znew = zeros(n);
            for (size_t i = 0; i < n; ++i)
                for (size_t j = 0; j < n; ++j)
                    for (size_t k2 = 0; k2 < n; ++k2) {
                        Ynew[i][j] += 0.5 * Y[i][k2] * M[k2][j];
                        Znew[i][j] += 0.5 * M[i][k2] * Z[k2][j];
                    }
            Y.swap(Ynew);
            Z.swap(Znew);
        }
        // Y approximates (cov)^{-1/2}
        T = mat_mul(Y, T);
        cur = transform(points, T);
        // Check convergence: max deviation of eigenvalues from 1 via trace
        Matrix new_cov = covariance(cur);
        double max_err = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double err = std::abs(new_cov[i][i] - 1.0);
            if (err > max_err) max_err = err;
        }
        if (max_err < tol) break;
    }
    return T;
}

// Utility to generate standard normal points
inline std::vector<Vector> random_normal_points(size_t m, size_t n, unsigned seed = 42) {
    std::mt19937 rng(seed);
    std::normal_distribution<double> nd(0.0, 1.0);
    std::vector<Vector> pts(m, Vector(n));
    for (auto& p : pts)
        for (double& v : p)
            v = nd(rng);
    return pts;
}

} // namespace iso
