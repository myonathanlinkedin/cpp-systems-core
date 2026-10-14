#include "types.hpp"
#include "engine.cpp"

#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

int main() {
    using namespace tlaplus;

    constexpr std::size_t kNumThreads = 5;
    constexpr std::size_t kIterationsPerThread = 10000;

    BakeryLock lock(kNumThreads);

    // Shared data protected by the lock.
    std::atomic<int> critical_counter{0};
    // Tracks how many threads are inside the critical section at any moment.
    std::atomic<int> inside_cs{0};

    // Vector to hold thread objects.
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);

    for (std::size_t tid = 0; tid < kNumThreads; ++tid) {
        threads.emplace_back([tid, &lock, &critical_counter, &inside_cs]() {
            for (std::size_t i = 0; i < kIterationsPerThread; ++i) {
                // Acquire lock via RAII guard.
                ScopedLock guard(lock, tid);

                // Begin critical section.
                int prev = inside_cs.fetch_add(1, std::memory_order_seq_cst);
                // Mutual exclusion invariant: prev must be 0.
                assert(prev == 0 && "Mutual exclusion violated: more than one thread entered CS");

                // Simulate work: increment a shared counter.
                critical_counter.fetch_add(1, std::memory_order_seq_cst);

                // End critical section.
                inside_cs.fetch_sub(1, std::memory_order_seq_cst);
                // guard destructor releases the lock.
            }
        });
    }

    // Join all threads.
    for (auto& t : threads) {
        t.join();
    }

    // Verify that the total number of increments matches expectation.
    const int expected = static_cast<int>(kNumThreads * kIterationsPerThread);
    assert(critical_counter.load(std::memory_order_seq_cst) == expected &&
           "Final counter does not match expected value");

    std::cout << "All tests passed. Final counter = " << critical_counter.load() << std::endl;
    return 0;
}
