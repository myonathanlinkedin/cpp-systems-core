#include <vector>

#include "core.hpp"
#include "types.hpp"
#include <algorithm>

static void transpose(const Matrix& src, Matrix& dst, size_t row_start, size_t col_start, size_t size) {
    if (size == 0) return;
    if (size == 1) {
        dst[col_start][row_start] = src[row_start][col_start];
        return;
    }
    size_t mid = size / 2;
    // top-left quadrant
    transpose(src, dst, row_start, col_start, mid);
    // top-right quadrant
    transpose(src, dst, row_start, col_start + mid, size - mid);
    // bottom-left quadrant
    transpose(src, dst, row_start + mid, col_start, size - mid);
    // bottom-right quadrant
    transpose(src, dst, row_start + mid, col_start + mid, size - mid);
}

Matrix cacheObliviousTranspose(const Matrix& src) {
    size_t n = src.size();
    Matrix dst(n, std::vector<int>(n));
    transpose(src, dst, 0, 0, n);
    return dst;
}
