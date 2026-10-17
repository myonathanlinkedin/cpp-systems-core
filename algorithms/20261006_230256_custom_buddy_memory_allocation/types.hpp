#pragma once
#include <cstddef>
#include <cstdint>

struct BlockHeader {
    uint8_t order;   // exponent of block size (2^order)
    bool is_free;
};
