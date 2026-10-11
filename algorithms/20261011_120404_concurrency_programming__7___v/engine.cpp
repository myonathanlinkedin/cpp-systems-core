#include <chrono>
#include <atomic>

#include "types.hpp"

namespace concurrency {

void VolatileFlag::set() {
    // Release fence before publishing the new value.
    std::atomic_thread_fence(std::memory_order_seq_cst);
    flag_ = true;
    // Ensure the store is not reordered after this point.
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

void VolatileFlag::clear() {
    // Ensure all prior writes are visible before clearing.
    std::atomic_thread_fence(std::memory_order_seq_cst);
    flag_ = false;
    // Prevent subsequent stores from moving before the clear.
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

bool VolatileFlag::is_set() const {
    // Acquire fence to synchronize with the release fence in set()/clear().
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return flag_;
}

void VolatileFlag::wait_until_set(std::chrono::milliseconds timeout) const {
    auto start = std::chrono::steady_clock::now();
    while (!is_set()) {
        if (timeout != std::chrono::milliseconds::max()) {
            auto now = std::chrono::steady_clock::now();
            if (now - start >= timeout) {
                break;
            }
        }
        // Yield to allow other threads to make progress.
        std::this_thread::yield();
    }
}

} // namespace concurrency
