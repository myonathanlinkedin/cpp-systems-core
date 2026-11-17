#include <cassert>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include <algorithm>

class RingBufferDispatcher {
public:
    struct PacketRef {
        const uint8_t* data = nullptr;
        std::size_t size = 0;
    };

    explicit RingBufferDispatcher(std::size_t capacity)
        : capacity_(capacity),
          buffer_(capacity),
          head_(0),
          tail_(0),
          count_(0) {
        // capacity must be >0
        assert(capacity_ > 0);
    }

    // Disable copy semantics, enable move
    RingBufferDispatcher(const RingBufferDispatcher&) = delete;
    RingBufferDispatcher& operator=(const RingBufferDispatcher&) = delete;
    RingBufferDispatcher(RingBufferDispatcher&&) noexcept = default;
    RingBufferDispatcher& operator=(RingBufferDispatcher&&) noexcept = default;

    // Enqueue a packet reference; returns false if buffer is full
    bool enqueue(const uint8_t* data, std::size_t size) noexcept {
        if (isFull()) return false;
        buffer_[tail_].data = data;
        buffer_[tail_].size = size;
        tail_ = (tail_ + 1) % capacity_;
        ++count_;
        return true;
    }

    // Dequeue the oldest packet; returns empty optional if buffer is empty
    std::optional<PacketRef> dequeue() noexcept {
        if (isEmpty()) return std::nullopt;
        PacketRef pkt = buffer_[head_];
        // Clear slot (optional, helps debugging)
        buffer_[head_].data = nullptr;
        buffer_[head_].size = 0;
        head_ = (head_ + 1) % capacity_;
        --count_;
        return pkt;
    }

    // Peek at the oldest packet without removing it
    std::optional<PacketRef> peek() const noexcept {
        if (isEmpty()) return std::nullopt;
        return buffer_[head_];
    }

    bool isEmpty() const noexcept { return count_ == 0; }
    bool isFull() const noexcept { return count_ == capacity_; }
    std::size_t size() const noexcept { return count_; }
    std::size_t capacity() const noexcept { return capacity_; }

private:
    std::size_t capacity_;
    std::vector<PacketRef> buffer_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;
};
