#include "types.hpp"
#include <cassert>
#include <cstring>

namespace mempool {

struct FreeNode {
    FreeNode* next;
};

template<std::size_t BlockSize, std::size_t BlockCount>
MemoryPool<BlockSize, BlockCount>::MemoryPool()
    : free_head_(nullptr)
{
    static_assert(BlockSize >= sizeof(FreeNode),
                  "BlockSize must be at least the size of a free‑list node.");

    // Initialise the free list: link each block to the next.
    for (std::size_t i = 0; i < BlockCount; ++i) {
        void* blk = block_at(i);
        auto* node = static_cast<FreeNode*>(blk);
        node->next = (i + 1 < BlockCount) ? static_cast<FreeNode*>(block_at(i + 1)) : nullptr;
    }
    free_head_ = static_cast<FreeNode*>(block_at(0));
}

template<std::size_t BlockSize, std::size_t BlockCount>
void* MemoryPool<BlockSize, BlockCount>::allocate()
{
    if (!free_head_) {
        // Pool exhausted.
        return nullptr;
    }
    // Pop the head of the free list.
    FreeNode* allocated = free_head_;
    free_head_ = free_head_->next;
    return static_cast<void*>(allocated);
}

template<std::size_t BlockSize, std::size_t BlockCount>
void MemoryPool<BlockSize, BlockCount>::deallocate(void* ptr)
{
    if (!ptr) {
        return; // No‑op for nullptr.
    }

    // Verify that `ptr` lies inside the buffer and is correctly aligned.
    std::uintptr_t start = reinterpret_cast<std::uintptr_t>(buffer_);
    std::uintptr_t end   = start + BlockSize * BlockCount;
    std::uintptr_t p    = reinterpret_cast<std::uintptr_t>(ptr);

    assert(p >= start && p < end && ((p - start) % BlockSize) == 0 &&
           "Pointer does not belong to this MemoryPool.");

    // Push the block back onto the free list.
    auto* node = static_cast<FreeNode*>(ptr);
    node->next = free_head_;
    free_head_ = node;
}

template<std::size_t BlockSize, std::size_t BlockCount>
void MemoryPool<BlockSize, BlockCount>::reset()
{
    // Re‑link all blocks exactly as in the constructor.
    for (std::size_t i = 0; i < BlockCount; ++i) {
        void* blk = block_at(i);
        auto* node = static_cast<FreeNode*>(blk);
        node->next = (i + 1 < BlockCount) ? static_cast<FreeNode*>(block_at(i + 1)) : nullptr;
    }
    free_head_ = static_cast<FreeNode*>(block_at(0));
}

template<std::size_t BlockSize, std::size_t BlockCount>
std::size_t MemoryPool<BlockSize, BlockCount>::used() const noexcept
{
    // Count blocks not on the free list.
    std::size_t free_cnt = 0;
    for (FreeNode* cur = free_head_; cur != nullptr; cur = cur->next) {
        ++free_cnt;
    }
    return BlockCount - free_cnt;
}

template<std::size_t BlockSize, std::size_t BlockCount>
void* MemoryPool<BlockSize, BlockCount>::block_at(std::size_t index) noexcept
{
    return static_cast<void*>(buffer_ + index * BlockSize);
}

// Explicit instantiation for the test harness (avoids linker errors when
// the template is used only in `main.cpp`).
template class MemoryPool<64, 1024>;

} // namespace mempool
