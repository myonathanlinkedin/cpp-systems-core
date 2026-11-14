#include <stdexcept>

#include "core.hpp"

namespace lockfree {

template <typename T>
RingBuffer<T>::RingBuffer(std::size_t capacity)
    : buffer_(capacity)
    , mask_(capacity - 1)
    , head_(0)
    , tail_(0)
{
    if (!is_power_of_two(capacity) || capacity < 2) {
        throw std::invalid_argument("capacity must be power of two and >= 2");
    }
}

// ----- try_push (lvalue) -----
template <typename T>
bool RingBuffer<T>::try_push(const T& item) {
    std::size_t cur_tail = tail_.load(std::memory_order_relaxed);
    std::size_t next_tail = (cur_tail + 1) & mask_;

    // Check for full condition.
    if (next_tail == head_.load(std::memory_order_acquire)) {
        return false; // Buffer full.
    }

    buffer_[cur_tail] = item; // Copy into slot.
    // Ensure the write to buffer_ happens before publishing the new tail.
    tail_.store(next_tail, std::memory_order_release);
    return true;
}

// ----- try_push (rvalue) -----
template <typename T>
bool RingBuffer<T>::try_push(T&& item) {
    std::size_t cur_tail = tail_.load(std::memory_order_relaxed);
    std::size_t next_tail = (cur_tail + 1) & mask_;

    if (next_tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    buffer_[cur_tail] = std::move(item);
    tail_.store(next_tail, std::memory_order_release);
    return true;
}

// ----- try_pop -----
template <typename T>
bool RingBuffer<T>::try_pop(T& out) {
    std::size_t cur_head = head_.load(std::memory_order_relaxed);
    if (cur_head == tail_.load(std::memory_order_acquire)) {
        return false; // Buffer empty.
    }

    out = std::move(buffer_[cur_head]); // Move out.
    std::size_t next_head = (cur_head + 1) & mask_;
    head_.store(next_head, std::memory_order_release);
    return true;
}

// Explicit instantiation for common types used in tests.
template class RingBuffer<int>;
template class RingBuffer<std::size_t>;

} // namespace lockfree
