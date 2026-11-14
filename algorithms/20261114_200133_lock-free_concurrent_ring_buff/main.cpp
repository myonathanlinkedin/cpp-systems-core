#include <atomic>

#include "core.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>

using lockfree::RingBuffer;

void single_thread_basic_test() {
    RingBuffer<int> rb(8); // capacity 8 (must be power of two)

    // Initially empty.
    assert(rb.empty());
    assert(!rb.full());
    assert(rb.size() == 0);

    // Push elements.
    for (int i = 0; i < 7; ++i) {
        bool ok = rb.try_push(i);
        assert(ok);
    }
    assert(!rb.empty());
    assert(rb.full()); // 7 elements fill a capacity‑8 SPSC ring (one slot left unused).

    // Attempt to push when full.
    assert(!rb.try_push(999));

    // Pop and verify order.
    for (int i = 0; i < 7; ++i) {
        int val = -1;
        bool ok = rb.try_pop(val);
        assert(ok);
        assert(val == i);
    }
    assert(rb.empty());
    assert(!rb.full());
    assert(rb.size() == 0);
}

void multi_thread_spsc_test() {
    const std::size_t capacity = 1024;
    RingBuffer<std::size_t> rb(capacity);

    const std::size_t total = 1'000'000;
    std::atomic<bool> producer_done{false};

    // Producer thread.
    std::thread prod([&]() {
        for (std::size_t i = 0; i < total; ++i) {
            // Spin until we can push.
            while (!rb.try_push(i)) {
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });

    // Consumer thread.
    std::thread cons([&]() {
        std::size_t expected = 0;
        while (true) {
            std::size_t val;
            if (rb.try_pop(val)) {
                assert(val == expected);
                ++expected;
            } else {
                if (producer_done.load(std::memory_order_acquire) && rb.empty()) {
                    break; // All items consumed.
                }
                std::this_thread::yield();
            }
        }
        assert(expected == total);
    });

    prod.join();
    cons.join();
}

int main() {
    std::cout << "Running single‑threaded basic test..." << std::endl;
    single_thread_basic_test();
    std::cout << "Single‑threaded test passed." << std::endl;

    std::cout << "Running multi‑threaded SPSC test..." << std::endl;
    multi_thread_spsc_test();
    std::cout << "Multi‑threaded test passed." << std::endl;

    std::cout << "All tests succeeded." << std::endl;
    return 0;
}
