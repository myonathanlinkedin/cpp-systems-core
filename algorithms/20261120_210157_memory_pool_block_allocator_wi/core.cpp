#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <cassert>

class MemoryPool {
public:
    // blockSize must be >= sizeof(void*) to store the free‑list pointer
    MemoryPool(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(std::max(blockSize, sizeof(void*))),
          blockCount_(blockCount),
          used_(0),
          buffer_(new std::uint8_t[blockSize_ * blockCount_]),
          freeList_(nullptr)
    {
        // initialise free list: each block stores a pointer to the next free block
        std::uint8_t* ptr = buffer_.get();
        for (std::size_t i = 0; i < blockCount_; ++i) {
            void* next = (i + 1 < blockCount_) ? static_cast<void*>(ptr + blockSize_) : nullptr;
            setNext(ptr, next);
            ptr += blockSize_;
        };
        freeList_ = buffer_.get();
    }

    ~MemoryPool() = default;

    // Allocate a block; returns nullptr if pool exhausted
    void* allocate() {
        if (!freeList_) return nullptr;
        void* result = freeList_;
        freeList_ = getNext(freeList_);
        ++used_;
        return result;
    }

    // Return a previously allocated block to the pool
    void deallocate(void* p) {
        assert(p != nullptr && "deallocate received nullptr");
        assert(owns(p) && "pointer does not belong to this pool");
        setNext(p, freeList_);
        freeList_ = p;
        assert(used_ > 0 && "deallocate called more times than allocate");
        --used_;
    }

    std::size_t capacity() const { return blockCount_; }
    std::size_t used() const { return used_; }
    std::size_t free() const { return blockCount_ - used_; }

    // Checks whether a pointer lies inside the pool and is correctly aligned
    bool owns(void* p) const {
        if (!p) return false;
        std::uintptr_t start = reinterpret_cast<std::uintptr_t>(buffer_.get());
        std::uintptr_t end   = start + blockSize_ * blockCount_;
        std::uintptr_t addr  = reinterpret_cast<std::uintptr_t>(p);
        if (addr < start || addr >= end) return false;
        return ((addr - start) % blockSize_) == 0;
    }

private:
    std::size_t blockSize_;
    std::size_t blockCount_;
    std::size_t used_;
    std::unique_ptr<std::uint8_t[]> buffer_;
    void* freeList_; // points to first free block

    static void* getNext(void* block) {
        return *static_cast<void**>(block);
    }
    static void setNext(void* block, void* next) {
        *static_cast<void**>(block) = next;
    }
};
