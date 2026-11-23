#pragma once
#include <cstddef>
#include <new>
#include <cassert>
#include <algorithm>

namespace mempool {

class MemoryPool {
public:
    // Construct a pool for `blockCount` blocks each of size `blockSize` bytes.
    // `blockSize` is rounded up to satisfy alignment of max_align_t.
    MemoryPool(std::size_t blockSize, std::size_t blockCount) noexcept
        : blockSize_(adjustBlockSize(blockSize)),
          blockCount_(blockCount),
          buffer_(nullptr),
          freeList_(nullptr),
          used_(0)
    {
        assert(blockCount_ > 0);
        // Allocate raw memory; use ::operator new for alignment guarantees.
        buffer_ = static_cast<char*>(::operator new(blockSize_ * blockCount_));
        // Initialise free list: each block stores a pointer to the next free block.
        for (std::size_t i = 0; i < blockCount_; ++i) {
            void* block = buffer_ + i * blockSize_;
            pushFreeBlock(block);
        };
    }

    // Non‑copyable, movable.
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept
        : blockSize_(other.blockSize_),
          blockCount_(other.blockCount_),
          buffer_(other.buffer_),
          freeList_(other.freeList_),
          used_(other.used_)
    {
        other.buffer_ = nullptr;
        other.freeList_ = nullptr;
        other.used_ = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept {
        if (this != &other) {
            release();
            blockSize_ = other.blockSize_;
            blockCount_ = other.blockCount_;
            buffer_ = other.buffer_;
            freeList_ = other.freeList_;
            used_ = other.used_;

            other.buffer_ = nullptr;
            other.freeList_ = nullptr;
            other.used_ = 0;
        }
        return *this;
    }

    ~MemoryPool() noexcept { release(); }

    // Allocate a single block; returns nullptr if pool exhausted.
    void* allocate() noexcept {
        if (!freeList_) return nullptr;               // pool empty
        void* result = freeList_;
        // Pop head of free list.
        freeList_ = static_cast<FreeNode*>(freeList_)->next;
        ++used_;
        return result;
    }

    // Return a previously allocated block to the pool.
    // Behaviour is undefined if `ptr` is not from this pool or already freed.
    void deallocate(void* ptr) noexcept {
        assert(ptr != nullptr);
        // Simple sanity check: pointer must lie inside the buffer range.
        assert(isFromPool(ptr));
        pushFreeBlock(ptr);
        --used_;
    }

    std::size_t capacity() const noexcept { return blockCount_; }
    std::size_t used() const noexcept { return used_; }
    std::size_t free() const noexcept { return blockCount_ - used_; }

private:
    struct FreeNode {
        FreeNode* next;
    };

    // Round up to multiple of alignof(std::max_align_t) to guarantee any type fits.
    static std::size_t adjustBlockSize(std::size_t sz) noexcept {
        constexpr std::size_t align = alignof(std::max_align_t);
        return ((sz + align - 1) / align) * align;
    }

    bool isFromPool(void* ptr) const noexcept {
        const char* p = static_cast<char*>(ptr);
        return p >= buffer_ && p < buffer_ + blockSize_ * blockCount_ &&
               ((p - buffer_) % blockSize_ == 0);
    }

    void pushFreeBlock(void* ptr) noexcept {
        auto node = static_cast<FreeNode*>(ptr);
        node->next = freeList_;
        freeList_ = node;
    }

    void release() noexcept {
        if (buffer_) {
            ::operator delete(buffer_);
            buffer_ = nullptr;
            freeList_ = nullptr;
            used_ = 0;
        }
    }

    std::size_t blockSize_;
    std::size_t blockCount_;
    char* buffer_;          // raw memory buffer
    FreeNode* freeList_;    // singly linked list of free blocks
    std::size_t used_;
};

} // namespace mempool
