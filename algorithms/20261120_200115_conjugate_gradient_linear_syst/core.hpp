#pragma once
#include "types.hpp"
#include <cstddef>

namespace cg {
Vector matVec(const Matrix& A, const Vector& x);
double dot(const Vector& a, const Vector& b);
double norm(const Vector& v);
Vector conjugateGradient(const Matrix& A,
                         const Vector& b,
                         const Vector& x0,
                         double tol = 1e-8,
                         int maxIter = 1000);
}
