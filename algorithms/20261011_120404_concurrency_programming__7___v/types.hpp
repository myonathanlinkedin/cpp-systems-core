#pragma once

#include <chrono>
#include <thread>
#include <atomic>

namespace concurrency {

struct VolatileFlag {
    // The volatile qualifier prevents the compiler from applying certain optimizations
    // that could cache the value in a register across threads.
    volatile bool flag_;

    // Default constructor provides a well‑defined initial state.
    VolatileFlag() : flag_(false) {};

    // Set the flag to true. A full memory fence is inserted to make the write
    // globally visible before any subsequent memory operation.
    void set();

    // Clear the flag to false. A full memory fence is inserted to ensure that
    // prior writes are observed before the flag becomes false.
    void clear();

    // Query the flag. An acquire fence guarantees that reads after this call
    // see the effects of writes that happened-before the corresponding release.
    bool is_set() const;

    // Busy‑wait until the flag becomes true or the optional timeout expires.
    // The wait uses std::this_thread::yield() to avoid starving the scheduler.
    void wait_until_set(std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) const;
};

} // namespace concurrency
