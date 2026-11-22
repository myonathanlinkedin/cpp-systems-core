#pragma once

#include <vector>
#include <cstddef>
#include <cassert>
#include <algorithm>

namespace cache_oblivious {

template <typename T>
class Transposer {
public:
    // Public interface: transposes an rows x cols matrix stored in row‑major order.
    // src must contain exactly rows*cols elements.
    // dst will be resized to cols*rows and hold the transposed matrix in row‑major order.
    static void transpose(const std::vector<T>& src,
                          std::vector<T>& dst,
                          std::size_t rows,
                          std::size_t cols) {
        assert(src.size() == rows * cols);
        dst.assign(cols * rows, T{});
        transposeRec(src, dst,
                     rows, cols,
                     0, 0,          // src start row, col
                     0, 0,          // dst start row, col
                     rows, cols);
    }

private:
    // Threshold for switching to the simple O(mn) kernel.
    static constexpr std::size_t BLOCK_SIZE = 16;

    // Recursive cache‑oblivious routine.
    static void transposeRec(const std::vector<T>& src,
                             std::vector<T>& dst,
                             std::size_t srcRows,
                             std::size_t srcCols,
                             std::size_t srcRowStart,
                             std::size_t srcColStart,
                             std::size_t dstRowStart,
                             std::size_t dstColStart,
                             std::size_t rows,
                             std::size_t cols) {
        if (rows <= BLOCK_SIZE && cols <= BLOCK_SIZE) {
            // Base case: direct element‑wise copy.
            for (std::size_t i = 0; i < rows; ++i) {
                for (std::size_t j = 0; j < cols; ++j) {
                    // Index in source (row‑major).
                    std::size_t srcIdx = (srcRowStart + i) * srcCols + (srcColStart + j);
                    // Index in destination (row‑major of transposed matrix).
                    // Destination dimensions are srcCols x srcRows.
                    std::size_t dstIdx = (dstRowStart + j) * srcRows + (dstColStart + i);
                    dst[dstIdx] = src[srcIdx];
                }
            }
            return;
        }

        // Split the larger dimension.
        if (rows >= cols) {
            std::size_t mid = rows / 2;
            // Upper half.
            transposeRec(src, dst,
                         srcRows, srcCols,
                         srcRowStart, srcColStart,
                         dstRowStart, dstColStart,
                         mid, cols);
            // Lower half.
            transposeRec(src, dst,
                         srcRows, srcCols,
                         srcRowStart + mid, srcColStart,
                         dstRowStart, dstColStart + mid,
                         rows - mid, cols);
        } else {
            std::size_t mid = cols / 2;
            // Left half.
            transposeRec(src, dst,
                         srcRows, srcCols,
                         srcRowStart, srcColStart,
                         dstRowStart, dstColStart,
                         rows, mid);
            // Right half.
            transposeRec(src, dst,
                         srcRows, srcCols,
                         srcRowStart, srcColStart + mid,
                         dstRowStart + mid, dstColStart,
                         rows, cols - mid);
        }
    }
};

} // namespace cache_oblivious
