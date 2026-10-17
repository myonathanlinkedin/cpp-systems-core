#pragma once
#include <cstddef>
#include <cstdint>
#include <array>

struct CuckooFilterConfig {
    std::size_t bucket_count = 1 << 16; // must be power of two
    std::size_t bucket_size = 4;        // entries per bucket
    std::size_t max_kicks = 500;        // relocation attempts
    std::size_t fingerprint_size = 1;  // bytes (8 bits)
};
