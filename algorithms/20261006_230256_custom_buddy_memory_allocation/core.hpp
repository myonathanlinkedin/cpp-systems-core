#pragma once
#include "types.hpp"
#include <vector>
#include <cstddef>

class BuddyAllocator {
public:
    explicit BuddyAllocator(uint8_t max_order);
    ~BuddyAllocator() = default;

    void* allocate(std::size_t size);
    void deallocate(void* ptr);
    std::size_t total_size() const;

private:
    uint8_t max_order_;
    std::vector<std::uint8_t> pool_;                     // raw memory
    std::vector<std::vector<std::size_t>> free_lists_;  // offsets per order

    std::size_t order_for_size(std::size_t size) const;
    std::size_t block_size(uint8_t order) const;
    std::size_t offset_of(void* ptr) const;
    void* ptr_of(std::size_t offset) const;
    void split_block(uint8_t from_order, std::size_t offset);
    void try_merge(std::size_t offset);
};
