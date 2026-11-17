#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace mempool {

/// Forward declaration of the internal free‑list node.
struct FreeNode;

/// A fixed‑size memory pool that hands out blocks of size `BlockSize`.
/// The pool owns a contiguous buffer of `BlockCount` blocks and maintains
/// a singly linked free list for O(1) allocation/deallocation.
///
/// Template Parameters
/// -------------------
/// * `BlockSize`  – size of each block in bytes (must be >= sizeof(FreeNode)).
/// * `BlockCount` – total number of blocks the pool can hold.
///
/// Guarantees
/// ---------
/// * No dynamic memory allocation after construction.
/// * Constant‑time allocate/deallocate.
/// * Thread‑unsafe (single‑threaded use only).
template<std::size_t BlockSize, std::size_t BlockCount>
class MemoryPool {
public:
    /// Construct the pool and initialise the free list.
    MemoryPool();

    /// Non‑copyable, non‑movable – the pool manages raw memory.
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    /// Allocate a block. Returns `nullptr` if the pool is exhausted.
    void* allocate();

    /// Return a previously allocated block to the pool.
    /// Passing `nullptr` is a no‑op. Supplying a pointer that does not
    /// belong to this pool triggers an assertion in debug builds.
    void deallocate(void* ptr);

    /// Reset the pool to its initial state (all blocks become free).
    void reset();

    /// Number of blocks the pool can hold.
    constexpr std::size_t capacity() const noexcept { return BlockCount; };

    /// Number of blocks currently allocated (i.e. not on the free list).
    std::size_t used() const noexcept;

private:
    /// Align the internal buffer to `alignof(std::max_align_t)` to satisfy
    /// any type that may be placed in the pool.
    alignas(std::max_align_t) std::byte buffer_[BlockSize * BlockCount];

    /// Head of the singly linked free list.
    FreeNode* free_head_;

    /// Helper to obtain a pointer to the i‑th block.
    void* block_at(std::size_t index) noexcept;
};

} // namespace mempool
