#include "core.hpp"
#include <cmath>
#include <stdexcept>

namespace cg {

Vector matVec(const Matrix& A, const Vector& x) {
    std::size_t n = A.size();
    if (x.size() != n) throw std::invalid_argument("Dimension mismatch");
    Vector y(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double sum = 0.0;
        const auto& row = A[i];
        for (std::size_t j = 0; j < n; ++j) {
            sum += row[j] * x[j];
        }
        y[i] = sum;
    }
    return y;
}

double dot(const Vector& a, const Vector& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Dimension mismatch");
    double res = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) res += a[i] * b[i];
    return res;
}

double norm(const Vector& v) {
    return std::sqrt(dot(v, v));
}

Vector conjugateGradient(const Matrix& A,
                         const Vector& b,
                         const Vector& x0,
                         double tol,
                         int maxIter) {
    std::size_t n = A.size();
    if (b.size() != n) throw std::invalid_argument("Dimension mismatch");
    Vector x = x0;
    if (x.empty()) x.assign(n, 0.0);
    Vector r = b;
    Vector Ax = matVec(A, x);
    for (std::size_t i = 0; i < n; ++i) r[i] -= Ax[i];
    Vector p = r;
    double rsold = dot(r, r);
    if (std::sqrt(rsold) < tol) return x;
    for (int k = 0; k < maxIter; ++k) {
        Vector Ap = matVec(A, p);
        double alpha = rsold / dot(p, Ap);
        for (std::size_t i = 0; i < n; ++i) x[i] += alpha * p[i];
        for (std::size_t i = 0; i < n; ++i) r[i] -= alpha * Ap[i];
        double rsnew = dot(r, r);
        if (std::sqrt(rsnew) < tol) break;
        double beta = rsnew / rsold;
        for (std::size_t i = 0; i < n; ++i) p[i] = r[i] + beta * p[i];
        rsold = rsnew;
    }
    return x;
}

} // namespace cg
