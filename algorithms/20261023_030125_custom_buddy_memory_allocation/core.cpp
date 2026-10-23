#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

namespace buddy {

struct Header {
    std::size_t order; // order of the block (log2 of size/minBlockSize)
};

class BuddyAllocator {
public:
    BuddyAllocator(std::size_t totalSize, std::size_t minBlockSize)
        : totalSize_(roundUpPowerOfTwo(totalSize)),
          minBlockSize_(roundUpPowerOfTwo(minBlockSize)),
          maxOrder_(calcMaxOrder()),
          buffer_(new std::uint8_t[totalSize_]),
          freeLists_(maxOrder_ + 1)
    {
        assert(isPowerOfTwo(totalSize_) && isPowerOfTwo(minBlockSize_));
        assert(minBlockSize_ <= totalSize_);
        // initially the whole buffer is one free block of maxOrder_
        freeLists_[maxOrder_].push_back(buffer_.get());
    };

    // Allocate memory of at least 'size' bytes. Returns nullptr on failure.
    void* allocate(std::size_t size) {
        // Account for header overhead
        std::size_t needed = size + sizeof(Header);
        std::size_t order = orderForSize(needed);
        if (order > maxOrder_) return nullptr; // too large

        std::size_t curOrder = order;
        while (curOrder <= maxOrder_ && freeLists_[curOrder].empty()) {
            ++curOrder;
        }
        if (curOrder > maxOrder_) return nullptr; // no block large enough

        // Split blocks down to desired order
        void* block = freeLists_[curOrder].back();
        freeLists_[curOrder].pop_back();

        while (curOrder > order) {
            --curOrder;
            std::size_t blockSize = blockSizeForOrder(curOrder);
            void* buddy = static_cast<std::uint8_t*>(block) + blockSize;
            freeLists_[curOrder].push_back(buddy);
        }

        // Write header and return user pointer
        Header* hdr = static_cast<Header*>(block);
        hdr->order = order;
        return static_cast<void*>(hdr + 1);
    }

    // Deallocate memory previously returned by allocate().
    void deallocate(void* ptr) {
        if (!ptr) return;
        Header* hdr = static_cast<Header*>(ptr) - 1;
        std::size_t order = hdr->order;
        void* block = static_cast<void*>(hdr);
        std::size_t blockSize = blockSizeForOrder(order);

        // Coalescing loop
        while (order < maxOrder_) {
            std::size_t offset = static_cast<std::uint8_t*>(block) - buffer_.get();
            std::size_t buddyOffset = offset ^ blockSize;
            void* buddy = buffer_.get() + buddyOffset;

            auto& list = freeLists_[order];
            auto it = std::find(list.begin(), list.end(), buddy);
            if (it == list.end()) {
                break; // buddy not free
            }
            // Remove buddy from free list
            list.erase(it);
            // Determine new block address (lower of the two)
            block = (buddyOffset < offset) ? buddy : block;
            ++order;
            blockSize <<= 1;
        }
        freeLists_[order].push_back(block);
    }

    // For testing: return total free memory in bytes.
    std::size_t totalFreeMemory() const {
        std::size_t sum = 0;
        for (std::size_t o = 0; o <= maxOrder_; ++o) {
            sum += freeLists_[o].size() * blockSizeForOrder(o);
        }
        return sum;
    }

private:
    std::size_t totalSize_;
    std::size_t minBlockSize_;
    std::size_t maxOrder_;
    std::unique_ptr<std::uint8_t[]> buffer_;
    std::vector<std::vector<void*>> freeLists_;

    static bool isPowerOfTwo(std::size_t x) {
        return x && ((x & (x - 1)) == 0);
    }

    static std::size_t roundUpPowerOfTwo(std::size_t x) {
        if (isPowerOfTwo(x)) return x;
        std::size_t p = 1;
        while (p < x) p <<= 1;
        return p;
    }

    std::size_t calcMaxOrder() const {
        std::size_t ratio = totalSize_ / minBlockSize_;
        std::size_t order = 0;
        while (ratio > 1) {
            ratio >>= 1;
            ++order;
        }
        return order;
    }

    std::size_t blockSizeForOrder(std::size_t order) const {
        return minBlockSize_ << order;
    }

    std::size_t orderForSize(std::size_t size) const {
        std::size_t block = minBlockSize_;
        std::size_t order = 0;
        while (block < size) {
            block <<= 1;
            ++order;
        }
        return order;
    }
};

} // namespace buddy
