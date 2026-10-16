#include <vector>

#include "core.hpp"
#include <array>
#include <algorithm>
#include <cstdint>

namespace cdc {

// Rabin polynomial (irreducible) used for rolling hash.
constexpr uint64_t POLY = 0x3DA3358B4DC173ULL;

// Precomputed table for fast byte removal in rolling hash.
using Table = std::array<uint64_t, 256>;

static Table precompute_out_table(std::size_t window_size) {
    Table table{};
    for (std::size_t i = 0; i < 256; ++i) {
        uint64_t val = static_cast<uint64_t>(i);
        for (std::size_t j = 0; j < window_size; ++j) {
            // Multiply by x (i.e., shift left) modulo POLY.
            if (val & (1ULL << 63)) {
                val = (val << 1) ^ POLY;
            } else {
                val <<= 1;
            }
        }
        table[i] = val;
    }
    return table;
}

// Update rolling hash with a new byte.
static inline uint64_t roll_hash(uint64_t hash, uint8_t out_byte, uint8_t in_byte,
                                 const Table& out_table, std::size_t window_size) {
    // Remove the outgoing byte.
    hash ^= out_table[out_byte];
    // Multiply by x (shift left) for each new byte.
    if (hash & (1ULL << 63)) {
        hash = (hash << 1) ^ POLY;
    } else {
        hash <<= 1;
    }
    // Add the incoming byte.
    hash ^= static_cast<uint64_t>(in_byte);
    return hash;
}

// Core CDC implementation.
std::vector<Chunk> chunk_boundaries(const std::vector<uint8_t>& data,
                                   const CDCParams& params) {
    const std::size_t n = data.size();
    std::vector<Chunk> result;
    if (n == 0) return result;

    // Mask to decide chunk boundary: avg_size is power‑of‑two friendly.
    // For non‑power‑of‑two we approximate by using the nearest lower power.
    std::size_t mask_bits = 0;
    std::size_t tmp = params.avg_size;
    while (tmp >>= 1) ++mask_bits;
    const uint64_t mask = (1ULL << mask_bits) - 1ULL;

    const Table out_table = precompute_out_table(params.window_size);
    uint64_t hash = 0;
    std::size_t start = 0;
    std::size_t i = 0;

    // Initialize the rolling window.
    std::vector<uint8_t> window;
    window.reserve(params.window_size);
    for (; i < std::min(params.window_size, n); ++i) {
        uint8_t b = data[i];
        // Simple forward hash (no removal yet).
        if (hash & (1ULL << 63)) {
            hash = (hash << 1) ^ POLY;
        } else {
            hash <<= 1;
        }
        hash ^= static_cast<uint64_t>(b);
        window.push_back(b);
    }

    for (; i < n; ++i) {
        uint8_t out_byte = window[i % params.window_size];
        uint8_t in_byte = data[i];
        hash = roll_hash(hash, out_byte, in_byte, out_table, params.window_size);
        window[i % params.window_size] = in_byte;

        std::size_t cur_len = i + 1 - start;
        bool cut = false;

        if (cur_len >= params.min_size) {
            if ((hash & mask) == 0) {
                cut = true; // content‑defined boundary
            } else if (cur_len >= params.max_size) {
                cut = true; // force cut at max size
            }
        }

        if (cut) {
            result.emplace_back(start, cur_len);
            start = i + 1;
            // Reset hash for next chunk.
            hash = 0;
            // Re‑initialize window for the new chunk.
            std::size_t remaining = std::min(params.window_size, n - start);
            window.clear();
            for (std::size_t j = 0; j < remaining; ++j) {
                uint8_t b = data[start + j];
                if (hash & (1ULL << 63)) {
                    hash = (hash << 1) ^ POLY;
                } else {
                    hash <<= 1;
                }
                hash ^= static_cast<uint64_t>(b);
                window.push_back(b);
            }
            i = start + remaining - 1; // will be incremented by loop
        }
    }

    // Emit final chunk if any data remains.
    if (start < n) {
        result.emplace_back(start, n - start);
    }
    return result;
}

} // namespace cdc
