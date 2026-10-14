#pragma once

#include <atomic>
#include <cstddef>
#include <vector>

namespace tlaplus {

// Forward declaration of the lock class.
class BakeryLock;

// RAII guard for the BakeryLock.
class ScopedLock {
public:
    ScopedLock(BakeryLock& lock, std::size_t thread_id) noexcept;
    ~ScopedLock();

    // non‑copyable
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

    // movable
    ScopedLock(ScopedLock&& other) noexcept;
    ScopedLock& operator=(ScopedLock&& other) noexcept;

private:
    BakeryLock* lock_;
    std::size_t thread_id_;
    bool owns_;
};

} // namespace tlaplus
