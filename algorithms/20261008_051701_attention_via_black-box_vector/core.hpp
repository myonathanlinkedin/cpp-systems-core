#pragma once
#include "types.hpp"

Vector dot(const Vector& a, const Vector& b);
Vector softmax(const Vector& scores);
Vector attention(const Vector& query,
                 const Matrix& keys,
                 const Matrix& values);
