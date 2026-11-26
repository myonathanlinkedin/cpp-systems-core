#include <atomic>

#include "types.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace lockfree {

// A lock‑free multi‑producer / multi‑consumer bounded ring buffer.
// Implementation follows Dmitry Vyukov's MPMC queue algorithm.
// The capacity must be a power of two; the constructor rounds up if necessary.
template <typename T>
class RingBuffer {
    static_assert(std::is_nothrow_move_constructible<T>::value ||
                  std::is_copy_constructible<T>::value,
                  "T must be movable or copyable without throwing.");

    struct Cell {
        std::atomic<std::size_t> sequence;
        typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    };

    // Power‑of‑two mask for fast modulo.
    const std::size_t mask_;
    std::unique_ptr<Cell[]> buffer_;

    // Positions for producers (enqueue) and consumers (dequeue).
    std::atomic<std::size_t> enqueuePos_;
    std::atomic<std::size_t> dequeuePos_;

    // Helper to construct an element in a cell.
    static void construct(Cell& cell, const T& value) {
        new (&cell.storage) T(value);
    }

    static void construct(Cell& cell, T&& value) {
        new (&cell.storage) T(std::move(value));
    }

    // Helper to destroy an element in a cell.
    static void destroy(Cell& cell) noexcept {
        reinterpret_cast<T*>(&cell.storage)->~T();
    }

    // Access the stored value.
    static T& value(Cell& cell) noexcept {
        return *reinterpret_cast<T*>(&cell.storage);
    }

public:
    // Constructs a ring buffer with at least `capacity` slots.
    // The actual capacity will be the next power of two.
    explicit RingBuffer(std::size_t capacity)
        : mask_(next_power_of_two(capacity) - 1),
          buffer_(new Cell[next_power_of_two(capacity)]),
          enqueuePos_(0),
          dequeuePos_(0) {
        const std::size_t actual = mask_ + 1;
        for (std::size_t i = 0; i < actual; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    // Non‑copyable, non‑movable (the internal atomics would be unsafe to copy).
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // Returns the usable capacity (i.e. number of elements that can be stored).
    std::size_t capacity() const noexcept {
        return mask_ + 1;
    }

    // Approximate number of elements currently stored.
    // This is only a snapshot; the value may change immediately after return.
    std::size_t size() const noexcept {
        std::size_t enq = enqueuePos_.load(std::memory_order_acquire);
        std::size_t deq = dequeuePos_.load(std::memory_order_acquire);
        return enq - deq;
    }

    // Attempts to enqueue `item`. Returns true on success, false if the buffer is full.
    bool try_enqueue(const T& item) {
        Cell* cell;
        std::size_t pos = enqueuePos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            std::size_t seq = cell->sequence.load(std::memory_order_acquire);
            std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);
            if (diff == 0) {
                if (enqueuePos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    break; // claimed slot
                }
            } else if (diff < 0) {
                return false; // full
            } else {
                pos = enqueuePos_.load(std::memory_order_relaxed);
            }
        }
        construct(*cell, item);
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool try_enqueue(T&& item) {
        Cell* cell;
        std::size_t pos = enqueuePos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            std::size_t seq = cell->sequence.load(std::memory_order_acquire);
            std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);
            if (diff == 0) {
                if (enqueuePos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                pos = enqueuePos_.load(std::memory_order_relaxed);
            }
        }
        construct(*cell, std::move(item));
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    // Attempts to dequeue an element into `item`. Returns true on success, false if empty.
    bool try_dequeue(T& item) {
        Cell* cell;
        std::size_t pos = dequeuePos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            std::size_t seq = cell->sequence.load(std::memory_order_acquire);
            std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1);
            if (diff == 0) {
                if (dequeuePos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    break; // claimed slot
                }
            } else if (diff < 0) {
                return false; // empty
            } else {
                pos = dequeuePos_.load(std::memory_order_relaxed);
            }
        }
        item = std::move(value(*cell));
        destroy(*cell);
        cell->sequence.store(pos + mask_ + 1, std::memory_order_release);
        return true;
    }

private:
    // Utility: round up to the next power of two (minimum 2).
    static std::size_t next_power_of_two(std::size_t n) {
        if (n == 0) return 2;
        --n;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        if (sizeof(std::size_t) == 8) {
            n |= n >> 32;
        }
        return ++n;
    }
};

} // namespace lockfree

// Explicit template instantiation for the test harness (optional, forces compilation).
template class lockfree::RingBuffer<lockfree::TestData>;
