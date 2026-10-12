#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

namespace cdc {

// Simple struct to hold a chunk's start offset and length.
struct Chunk {
    std::size_t offset; // start index in the original buffer
    std::size_t length; // length of the chunk in bytes
    Chunk() = default;
    Chunk(std::size_t o, std::size_t l) : offset(o), length(l) {};
};

} // namespace cdc
