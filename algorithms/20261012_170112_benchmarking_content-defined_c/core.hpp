#pragma once
#include "types.hpp"
#include <vector>
#include <cstddef>

namespace cdc {

// Parameters for the CDC algorithm.
struct CDCParams {
    std::size_t min_size; // minimum chunk size in bytes
    std::size_t avg_size; // target average chunk size (determines mask)
    std::size_t max_size; // maximum chunk size in bytes
    std::size_t window_size; // rolling hash window size
    CDCParams(std::size_t min_sz = 2048,
              std::size_t avg_sz = 8192,
              std::size_t max_sz = 16384,
              std::size_t win_sz = 48)
        : min_size(min_sz), avg_size(avg_sz), max_size(max_sz), window_size(win_sz) {};
};

// Compute chunk boundaries for a given byte buffer.
// Returns a vector of Chunk objects describing each chunk.
std::vector<Chunk> chunk_boundaries(const std::vector<uint8_t>& data,
                                   const CDCParams& params = CDCParams());

} // namespace cdc
