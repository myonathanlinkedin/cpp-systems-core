#pragma once

#include <atomic>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace lockfree {

/**
 * @brief A lock‑free single‑producer single‑consumer (SPSC) ring buffer.
 *
 * The buffer size must be a power of two; this enables fast index wrapping
 * using a bit mask. All operations are wait‑free and use only std::atomic
 * with acquire/release semantics.
 *
 * @tparam T Element type. Must be copy‑constructible or movable.
 */
template <typename T>
class RingBuffer {
public:
    /**
     * @brief Construct a ring buffer with the given capacity.
     *
     * @param capacity Number of elements the buffer can hold. Must be a power of two
     *                 and at least 2. The actual usable capacity is `capacity`.
     *
     * @throws std::invalid_argument if capacity is not a power of two or < 2.
     */
    explicit RingBuffer(std::size_t capacity);

    /** Default destructor. */
    ~RingBuffer() = default;

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    /**
     * @brief Attempt to push a value into the buffer.
     *
     * @param item Value to insert (copy or move).
     * @return true if the item was inserted, false if the buffer was full.
     */
    bool try_push(const T& item);
    bool try_push(T&& item);

    /**
     * @brief Attempt to pop a value from the buffer.
     *
     * @param out Reference where the popped value will be stored.
     * @return true if a value was retrieved, false if the buffer was empty.
     */
    bool try_pop(T& out);

    /** @brief Return the total capacity of the buffer. */
    std::size_t capacity() const noexcept { return mask_ + 1; };

    /** @brief Return true if the buffer is empty. */
    bool empty() const noexcept { return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire); }

    /** @brief Return true if the buffer is full. */
    bool full() const noexcept {
        std::size_t next = (tail_.load(std::memory_order_acquire) + 1) & mask_;
        return next == head_.load(std::memory_order_acquire);
    }

    /** @brief Approximate number of stored elements (may be stale). */
    std::size_t size() const noexcept {
        std::size_t h = head_.load(std::memory_order_acquire);
        std::size_t t = tail_.load(std::memory_order_acquire);
        return (t + capacity() - h) & mask_;
    }

private:
    // Helper to check power‑of‑two.
    static bool is_power_of_two(std::size_t n) noexcept { return n && ((n & (n - 1)) == 0); }

    std::vector<T> buffer_;          // Underlying storage.
    const std::size_t mask_;         // capacity - 1, for fast wrap‑around.
    alignas(64) std::atomic<std::size_t> head_; // Consumer index.
    alignas(64) std::atomic<std::size_t> tail_; // Producer index.
};

} // namespace lockfree
