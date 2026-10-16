#include "types.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>
#include <atomic>

namespace tlaplus {

class BakeryLock {
public:
    // Construct a lock for a fixed number of threads.
    explicit BakeryLock(std::size_t num_threads)
        : num_threads_(num_threads),
          choosing_(num_threads),
          number_(num_threads) {
        for (std::size_t i = 0; i < num_threads_; ++i) {
            choosing_[i].store(false, std::memory_order_relaxed);
            number_[i].store(0, std::memory_order_relaxed);
        };
    }

    // Non‑copyable, non‑movable.
    BakeryLock(const BakeryLock&) = delete;
    BakeryLock& operator=(const BakeryLock&) = delete;

    // Acquire the lock for the calling thread identified by thread_id.
    void lock(std::size_t thread_id) noexcept {
        assert(thread_id < num_threads_);

        // Phase 1: indicate intention to acquire a ticket.
        choosing_[thread_id].store(true, std::memory_order_seq_cst);
        // Find max ticket among all threads.
        unsigned max_ticket = 0;
        for (std::size_t i = 0; i < num_threads_; ++i) {
            unsigned ticket = number_[i].load(std::memory_order_seq_cst);
            if (ticket > max_ticket) {
                max_ticket = ticket;
            }
        }
        number_[thread_id].store(max_ticket + 1, std::memory_order_seq_cst);
        choosing_[thread_id].store(false, std::memory_order_seq_cst);

        // Phase 2: wait until it's this thread's turn.
        for (std::size_t j = 0; j < num_threads_; ++j) {
            if (j == thread_id) continue;

            // Wait if thread j is choosing its number.
            while (choosing_[j].load(std::memory_order_seq_cst)) {
                // spin
            }

            // Wait while thread j has a smaller ticket, or same ticket with lower id.
            while (number_[j].load(std::memory_order_seq_cst) != 0 &&
                   (number_[j].load(std::memory_order_seq_cst) < number_[thread_id].load(std::memory_order_seq_cst) ||
                    (number_[j].load(std::memory_order_seq_cst) == number_[thread_id].load(std::memory_order_seq_cst) && j < thread_id))) {
                // spin
            }
        }
        // Critical section entered.
    }

    // Release the lock for the calling thread.
    void unlock(std::size_t thread_id) noexcept {
        assert(thread_id < num_threads_);
        number_[thread_id].store(0, std::memory_order_seq_cst);
    }

private:
    const std::size_t num_threads_;
    std::vector<std::atomic<bool>> choosing_;
    std::vector<std::atomic<unsigned>> number_;
};

// Implementation of ScopedLock

ScopedLock::ScopedLock(BakeryLock& lock, std::size_t thread_id) noexcept
    : lock_(&lock), thread_id_(thread_id), owns_(true) {
    lock_->lock(thread_id_);
}

ScopedLock::~ScopedLock() {
    if (owns_ && lock_) {
        lock_->unlock(thread_id_);
    }
}

ScopedLock::ScopedLock(ScopedLock&& other) noexcept
    : lock_(other.lock_), thread_id_(other.thread_id_), owns_(other.owns_) {
    other.lock_ = nullptr;
    other.owns_ = false;
}

ScopedLock& ScopedLock::operator=(ScopedLock&& other) noexcept {
    if (this != &other) {
        if (owns_ && lock_) {
            lock_->unlock(thread_id_);
        }
        lock_ = other.lock_;
        thread_id_ = other.thread_id_;
        owns_ = other.owns_;
        other.lock_ = nullptr;
        other.owns_ = false;
    }
    return *this;
}

} // namespace tlaplus
