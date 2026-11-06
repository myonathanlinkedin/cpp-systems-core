#include <cassert>
#include <algorithm>

#include "types.hpp"

namespace mempool {

std::size_t MemoryPool::alignedSize(std::size_t size) noexcept {
    constexpr std::size_t alignment = alignof(std::max_align_t);
    return (size + alignment - 1) & ~(alignment - 1);
}

MemoryPool::MemoryPool(std::size_t blockSize, std::size_t blockCount)
    : blockSize_(std::max(alignedSize(blockSize), alignedSize(sizeof(BlockHeader)))),
      blockCount_(blockCount),
      freeCount_(blockCount),
      buffer_(blockSize_ * blockCount_),
      freeList_(nullptr)
{
    // Initialise the free list; each block starts with a BlockHeader.
    std::uint8_t* raw = buffer_.data();
    for (std::size_t i = 0; i < blockCount_; ++i) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(raw + i * blockSize_);
        header->next = freeList_;
        freeList_ = header;
    }
}

void* MemoryPool::allocate() {
    if (!freeList_) {
        return nullptr; // Pool exhausted.
    }
    BlockHeader* head = freeList_;
    freeList_ = head->next;
    --freeCount_;
    // Return pointer just after the header so the user gets a clean block.
    return static_cast<void*>(head);
}

void MemoryPool::deallocate(void* ptr) {
    if (!ptr) {
        return; // No‑op for nullptr.
    }
    // Verify that ptr lies within the buffer and is correctly aligned.
    std::uintptr_t start = reinterpret_cast<std::uintptr_t>(buffer_.data());
    std::uintptr_t end   = start + buffer_.size();
    std::uintptr_t p    = reinterpret_cast<std::uintptr_t>(ptr);
    assert(p >= start && p < end && ((p - start) % blockSize_) == 0 && "Pointer does not belong to this pool");

    BlockHeader* header = static_cast<BlockHeader*>(ptr);
    header->next = freeList_;
    freeList_ = header;
    ++freeCount_;
}

} // namespace mempool
