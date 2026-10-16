#include <algorithm>

#include "core.hpp"
#include <cassert>
#include <cstring>

BuddyAllocator::BuddyAllocator(uint8_t max_order)
    : max_order_(max_order),
      pool_(1ULL << max_order, 0),
      free_lists_(max_order + 1) {
    // Initialize the whole memory as one free block
    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(pool_.data());
    hdr->order = max_order_;
    hdr->is_free = true;
    free_lists_[max_order_].push_back(0);
}

std::size_t BuddyAllocator::total_size() const {
    return pool_.size();
}

std::size_t BuddyAllocator::block_size(uint8_t order) const {
    return static_cast<std::size_t>(1) << order;
}

std::size_t BuddyAllocator::order_for_size(std::size_t size) const {
    // Include header size
    size += sizeof(BlockHeader);
    uint8_t order = 0;
    while (block_size(order) < size) ++order;
    return order;
}

void* BuddyAllocator::ptr_of(std::size_t offset) const {
    return const_cast<void*>(static_cast<const void*>(pool_.data() + offset));
}

std::size_t BuddyAllocator::offset_of(void* ptr) const {
    return static_cast<std::uint8_t*>(ptr) - pool_.data();
}

void BuddyAllocator::split_block(uint8_t from_order, std::size_t offset) {
    assert(from_order > 0);
    // Remove the larger block from its free list
    auto& src = free_lists_[from_order];
    auto it = std::find(src.begin(), src.end(), offset);
    assert(it != src.end());
    src.erase(it);

    uint8_t to_order = from_order - 1;
    std::size_t half = block_size(to_order);
    // Left buddy
    BlockHeader* left = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
    left->order = to_order;
    left->is_free = true;
    free_lists_[to_order].push_back(offset);
    // Right buddy
    std::size_t right_offset = offset + half;
    BlockHeader* right = reinterpret_cast<BlockHeader*>(pool_.data() + right_offset);
    right->order = to_order;
    right->is_free = true;
    free_lists_[to_order].push_back(right_offset);
}

void* BuddyAllocator::allocate(std::size_t size) {
    uint8_t needed_order = order_for_size(size);
    if (needed_order > max_order_) return nullptr;

    uint8_t cur_order = needed_order;
    while (cur_order <= max_order_ && free_lists_[cur_order].empty()) {
        ++cur_order;
    }
    if (cur_order > max_order_) return nullptr; // out of memory

    // Obtain a block of cur_order and split down to needed_order
    std::size_t offset = free_lists_[cur_order].back();
    free_lists_[cur_order].pop_back();

    while (cur_order > needed_order) {
        --cur_order;
        std::size_t half = block_size(cur_order);
        // Left buddy stays at current offset, right buddy is created
        BlockHeader* left = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
        left->order = cur_order;
        left->is_free = false; // will be allocated after final split
        std::size_t right_offset = offset + half;
        BlockHeader* right = reinterpret_cast<BlockHeader*>(pool_.data() + right_offset);
        right->order = cur_order;
        right->is_free = true;
        free_lists_[cur_order].push_back(right_offset);
    }

    // Mark final block as allocated
    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
    hdr->is_free = false;
    hdr->order = needed_order;

    // Return pointer after header
    return static_cast<void*>(pool_.data() + offset + sizeof(BlockHeader));
}

void BuddyAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    std::size_t offset = offset_of(ptr) - sizeof(BlockHeader);
    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
    assert(!hdr->is_free);
    hdr->is_free = true;
    free_lists_[hdr->order].push_back(offset);
    try_merge(offset);
}

void BuddyAllocator::try_merge(std::size_t offset) {
    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
    uint8_t order = hdr->order;

    while (order < max_order_) {
        std::size_t buddy_offset = offset ^ block_size(order);
        BlockHeader* buddy_hdr = reinterpret_cast<BlockHeader*>(pool_.data() + buddy_offset);
        if (!buddy_hdr->is_free || buddy_hdr->order != order) break;

        // Remove buddy from free list
        auto& list = free_lists_[order];
        auto it = std::find(list.begin(), list.end(), buddy_offset);
        if (it == list.end()) break;
        list.erase(it);

        // Remove current block from free list (it is at the back)
        auto& cur_list = free_lists_[order];
        it = std::find(cur_list.begin(), cur_list.end(), offset);
        if (it != cur_list.end()) cur_list.erase(it);

        // Merge to higher order
        offset = std::min(offset, buddy_offset);
        ++order;
        BlockHeader* merged = reinterpret_cast<BlockHeader*>(pool_.data() + offset);
        merged->order = order;
        merged->is_free = true;
        free_lists_[order].push_back(offset);
        hdr = merged;
    }
}
