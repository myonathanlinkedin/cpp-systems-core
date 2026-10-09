#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

void test_single_thread() {
    BoundedBlockingQueue q(5);
    assert(q.empty());
    q.push(10);
    assert(!q.empty());
    assert(q.size() == 1);
    int val = q.pop();
    assert(val == 10);
    assert(q.empty());
}

void test_multi_thread() {
    const std::size_t capacity = 3;
    BoundedBlockingQueue q(capacity);
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};
    const int total = 10;

    std::thread producer([&]() {
        for (int i = 0; i < total; ++i) {
            q.push(i);
            ++produced;
        }
    });

    std::thread consumer([&]() {
        for (int i = 0; i < total; ++i) {
            int val = q.pop();
            assert(val == i);
            ++consumed;
        }
    });

    producer.join();
    consumer.join();

    assert(produced == total);
    assert(consumed == total);
    assert(q.empty());
}

int main() {
    test_single_thread();
    test_multi_thread();
    std::cout << "All tests passed.\n";
    return 0;
}
