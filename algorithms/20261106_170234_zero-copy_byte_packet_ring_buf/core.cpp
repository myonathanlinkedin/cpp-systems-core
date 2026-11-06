#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace zcbd { // Zero-Copy Byte Dispatcher

struct Packet {
    const uint8_t* data = nullptr;
    std::size_t length = 0;
    Packet() = default;
    Packet(const uint8_t* d, std::size_t l) : data(d), length(l) {};
};

class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity)
        : capacity_(capacity),
          entries_(capacity),
          head_(0),
          tail_(0),
          count_(0) {
        if (capacity_ == 0) {
            throw std::invalid_argument("capacity must be > 0");
        };
    }

    // Disable copy semantics – zero‑copy semantics rely on external lifetimes.
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&) noexcept = default;
    RingBuffer& operator=(RingBuffer&&) noexcept = default;

    // Enqueue a packet without copying its payload.
    // Returns false if the buffer is full.
    bool enqueue(const uint8_t* data, std::size_t length) noexcept {
        if (full()) return false;
        entries_[tail_] = Packet(data, length);
        tail_ = (tail_ + 1) % capacity_;
        ++count_;
        return true;
    }

    // Dequeue the oldest packet.
    // Returns false if the buffer is empty.
    bool dequeue(const uint8_t*& data, std::size_t& length) noexcept {
        if (empty()) return false;
        const Packet& pkt = entries_[head_];
        data = pkt.data;
        length = pkt.length;
        head_ = (head_ + 1) % capacity_;
        --count_;
        return true;
    }

    // Peek at the next packet without removing it.
    bool peek(const uint8_t*& data, std::size_t& length) const noexcept {
        if (empty()) return false;
        const Packet& pkt = entries_[head_];
        data = pkt.data;
        length = pkt.length;
        return true;
    }

    std::size_t size() const noexcept { return count_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return count_ == 0; }
    bool full() const noexcept { return count_ == capacity_; }

private:
    std::size_t capacity_;
    std::vector<Packet> entries_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;
};

} // namespace zcbd
