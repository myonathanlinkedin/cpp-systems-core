#include <cassert>
#include <algorithm>

#include "types.hpp"

namespace buddy {

BuddyAllocator::BuddyAllocator(std::size_t min_order, std::size_t max_order)
    : min_order_(min_order), max_order_(max_order), free_sets_(max_order + 1) {
    assert(min_order_ <= max_order_);
    // Allocate the raw memory pool.
    memory_.resize(block_size(max_order_));
    // Initially the whole pool is a single free block at max_order.
    free_sets_[max_order_].insert(0);
}

std::size_t BuddyAllocator::order_for_size(std::size_t size) const {
    // Account for header overhead.
    std::size_t needed = size + sizeof(BlockHeader);
    std::size_t order = min_order_;
    while (order <= max_order_ && block_size(order) < needed) {
        ++order;
    }
    return (order > max_order_) ? max_order_ + 1 : order;
}

void* BuddyAllocator::allocate(std::size_t size) {
    if (size == 0) {
        return nullptr;
    }
    std::size_t target_order = order_for_size(size);
    if (target_order > max_order_) {
        return nullptr; // Too large.
    }

    // Find the smallest free block that can satisfy the request.
    std::size_t cur_order = target_order;
    while (cur_order <= max_order_ && free_sets_[cur_order].empty()) {
        ++cur_order;
    }
    if (cur_order > max_order_) {
        return nullptr; // No suitable block.
    }

    // Remove a block from the free set.
    std::size_t offset = *free_sets_[cur_order].begin();
    free_sets_[cur_order].erase(free_sets_[cur_order].begin());

    // Split until we reach the target order.
    while (cur_order > target_order) {
        --cur_order;
        std::size_t buddy = offset + block_size(cur_order);
        free_sets_[cur_order].insert(buddy);
    }

    // Write header.
    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(memory_.data() + offset);
    hdr->order = static_cast<uint8_t>(target_order);
    // Return pointer just after the header.
    return static_cast<void*>(memory_.data() + offset + sizeof(BlockHeader));
}

void BuddyAllocator::deallocate(void* ptr) {
    assert(ptr != nullptr);
    std::uint8_t* raw = static_cast<std::uint8_t*>(ptr);
    // Compute offset of the header.
    std::size_t offset = static_cast<std::size_t>(raw - memory_.data()) - sizeof(BlockHeader);
    assert(offset < memory_.size());

    BlockHeader* hdr = reinterpret_cast<BlockHeader*>(memory_.data() + offset);
    std::size_t order = static_cast<std::size_t>(hdr->order);
    assert(order >= min_order_ && order <= max_order_);

    // Coalescing loop.
    while (order < max_order_) {
        std::size_t buddy = buddy_offset(offset, order);
        auto& buddy_set = free_sets_[order];
        auto it = buddy_set.find(buddy);
        if (it == buddy_set.end()) {
            break; // Buddy not free.
        }
        // Remove buddy and merge.
        buddy_set.erase(it);
        offset = std::min(offset, buddy);
        ++order;
    }
    free_sets_[order].insert(offset);
}

std::size_t BuddyAllocator::free_count(std::size_t order) const {
    assert(order <= max_order_);
    return free_sets_[order].size();
}

} // namespace buddy
