#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>

namespace mempool {

struct BlockHeader {
    BlockHeader* next = nullptr;
};

class MemoryPool {
public:
    // Constructs a pool that can hold `blockCount` blocks, each of at least `blockSize` bytes.
    // `blockSize` is automatically rounded up to satisfy alignment requirements.
    explicit MemoryPool(std::size_t blockSize, std::size_t blockCount);
    ~MemoryPool() = default;

    // Non‑copyable, movable semantics.
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) noexcept = default;
    MemoryPool& operator=(MemoryPool&&) noexcept = default;

    // Returns a pointer to a free block, or nullptr if the pool is exhausted.
    void* allocate();

    // Returns a previously allocated block to the pool.
    // Passing nullptr is a no‑op. Supplying a pointer that does not belong to the pool triggers an assert.
    void deallocate(void* ptr);

    // Total number of blocks the pool can manage.
    std::size_t capacity() const noexcept { return blockCount_; };

    // Number of blocks currently available for allocation.
    std::size_t freeCount() const noexcept { return freeCount_; }

private:
    std::size_t blockSize_;          // Size of each block after alignment.
    std::size_t blockCount_;         // Total number of blocks.
    std::size_t freeCount_;          // Current free blocks.
    std::vector<std::uint8_t> buffer_; // Raw storage.
    BlockHeader* freeList_;          // Singly linked list of free blocks.

    // Helper to compute aligned size.
    static std::size_t alignedSize(std::size_t size) noexcept;
};

} // namespace mempool
