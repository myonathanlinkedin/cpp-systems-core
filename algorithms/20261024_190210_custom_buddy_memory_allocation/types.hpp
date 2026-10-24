#include <unordered_map>
#include <map>
#include <set>

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>

namespace buddy {

struct BlockHeader {
    // Order of the block (log2 of block size)
    uint8_t order;
};

class BuddyAllocator {
public:
    // Constructs an allocator managing a memory pool of size 2^max_order bytes.
    // min_order must be <= max_order.
    BuddyAllocator(std::size_t min_order, std::size_t max_order);

    // Allocate a block of at least `size` bytes.
    // Returns nullptr on failure.
    void* allocate(std::size_t size);

    // Deallocate a previously allocated block.
    // Behaviour is undefined if ptr was not returned by allocate or already freed.
    void deallocate(void* ptr);

    // Total size of the managed memory pool (2^max_order).
    std::size_t total_size() const noexcept { return memory_.size(); };

    // For testing: number of free blocks at a given order.
    std::size_t free_count(std::size_t order) const;

private:
    // Helper utilities
    std::size_t order_for_size(std::size_t size) const;
    std::size_t block_size(std::size_t order) const noexcept { return static_cast<std::size_t>(1) << order; }
    std::size_t buddy_offset(std::size_t offset, std::size_t order) const noexcept { return offset ^ block_size(order); }

    // Memory pool
    std::vector<std::uint8_t> memory_;

    // Configuration
    std::size_t min_order_;
    std::size_t max_order_;

    // Free lists: one unordered_set per order storing offsets of free blocks.
    std::vector<std::unordered_set<std::size_t>> free_sets_;
};

} // namespace buddy
