#include <vector>

#include "core.hpp"

namespace lsm {

static void transpose_recursive(const Matrix& src, Matrix& dst,
                                 std::size_t r0, std::size_t r1,
                                 std::size_t c0, std::size_t c1) {
    std::size_t rows = r1 - r0;
    std::size_t cols = c1 - c0;
    if (rows == 0 || cols == 0) {
        return;
    }
    // Base case: small submatrix, perform naive copy
    if (rows <= 64 && cols <= 64) {
        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < cols; ++j) {
                dst[c0 + j][r0 + i] = src[r0 + i][c0 + j];
            }
        }
        return;
    }
    // Recursive division
    if (rows > cols) {
        std::size_t mid = r0 + rows / 2;
        transpose_recursive(src, dst, r0, mid, c0, c1);
        transpose_recursive(src, dst, mid, r1, c0, c1);
    } else {
        std::size_t mid = c0 + cols / 2;
        transpose_recursive(src, dst, r0, r1, c0, mid);
        transpose_recursive(src, dst, r0, r1, mid, c1);
    }
}

void transpose(const Matrix& src, Matrix& dst) {
    std::size_t rows = src.size();
    std::size_t cols = rows > 0 ? src[0].size() : 0;
    dst.assign(cols, std::vector<int>(rows));
    transpose_recursive(src, dst, 0, rows, 0, cols);
}

} // namespace lsm
