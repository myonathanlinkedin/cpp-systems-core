#include <atomic>

#include "types.hpp"
#include "engine.cpp"   // Include implementation directly for a single‑translation‑unit build.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

using namespace lockfree;

void single_threaded_basic_test() {
    RingBuffer<TestData> rb(4); // capacity will be 4 (already power of two)

    // Buffer should be empty initially.
    assert(rb.size() == 0);
    TestData out;

    // Dequeue from empty buffer must fail.
    assert(!rb.try_dequeue(out));

    // Enqueue up to capacity.
    for (int i = 0; i < 4; ++i) {
        TestData td{i, i * 10};
        assert(rb.try_enqueue(td));
    }

    // Buffer now full; further enqueue must fail.
    assert(!rb.try_enqueue(TestData{99, 999}));

    // Dequeue all items and verify order.
    for (int i = 0; i < 4; ++i) {
        assert(rb.try_dequeue(out));
        assert(out.id == i);
        assert(out.value == i * 10);
    }

    // Buffer empty again.
    assert(!rb.try_dequeue(out));
    assert(rb.size() == 0);
}

void multi_threaded_stress_test() {
    const std::size_t capacity = 1024;
    const std::size_t producer_count = 4;
    const std::size_t consumer_count = 4;
    const std::size_t items_per_producer = 100000;

    RingBuffer<TestData> rb(capacity);

    std::atomic<std::size_t> produced{0};
    std::atomic<std::size_t> consumed{0};

    // Producer threads.
    std::vector<std::thread> producers;
    for (std::size_t p = 0; p < producer_count; ++p) {
        producers.emplace_back([&, p]() {
            for (std::size_t i = 0; i < items_per_producer; ++i) {
                TestData td{static_cast<int>(p), static_cast<int>(i)};
                // Spin until we succeed (buffer may be temporarily full).
                while (!rb.try_enqueue(td)) {
                    std::this_thread::yield();
                }
                ++produced;
            }
        });
    }

    // Consumer threads.
    std::vector<std::thread> consumers;
    for (std::size_t c = 0; c < consumer_count; ++c) {
        consumers.emplace_back([&]() {
            TestData td;
            while (consumed.load(std::memory_order_relaxed) <
                   producer_count * items_per_producer) {
                if (rb.try_dequeue(td)) {
                    ++consumed;
                    // Simple verification: id must be < producer_count.
                    assert(td.id >= 0 && static_cast<std::size_t>(td.id) < producer_count);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& th : producers) th.join();
    for (auto& th : consumers) th.join();

    // All items must have been transferred.
    assert(produced.load() == producer_count * items_per_producer);
    assert(consumed.load() == produced.load());
    // Buffer must be empty.
    TestData dummy;
    assert(!rb.try_dequeue(dummy));
}

int main() {
    std::cout << "Running single‑threaded basic test..." << std::endl;
    single_threaded_basic_test();
    std::cout << "Single‑threaded test passed.\n" << std::endl;

    std::cout << "Running multi‑threaded stress test..." << std::endl;
    multi_threaded_stress_test();
    std::cout << "Multi‑threaded test passed.\n" << std::endl;

    std::cout << "All tests succeeded.\n";
    return 0;
}
