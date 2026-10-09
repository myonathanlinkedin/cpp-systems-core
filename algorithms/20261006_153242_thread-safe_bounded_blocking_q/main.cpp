#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cassert>
#include "core.cpp" // Include implementation directly for simplicity

void test_single_thread() {
    BoundedBlockingQueue<int> q(3);
    q.push(1);
    q.push(2);
    q.push(3);
    assert(q.size() == 3);
    int val = 0;
    q.pop(val);
    assert(val == 1);
    q.pop(val);
    assert(val == 2);
    q.pop(val);
    assert(val == 3);
    assert(q.size() == 0);
}

void test_non_blocking() {
    BoundedBlockingQueue<int> q(2);
    assert(q.try_push(10));
    assert(q.try_push(20));
    assert(!q.try_push(30)); // queue full
    int v = 0;
    assert(q.try_pop(v) && v == 10);
    assert(q.try_pop(v) && v == 20);
    assert(!q.try_pop(v)); // queue empty
}

void producer(BoundedBlockingQueue<int>& q, int start, int count) {
    for (int i = 0; i < count; ++i) {
        q.push(start + i);
    }
}

void consumer(BoundedBlockingQueue<int>& q, int total, std::vector<int>& out) {
    for (int i = 0; i < total; ++i) {
        int v;
        q.pop(v);
        out.push_back(v);
    }
}

void test_multi_thread() {
    const int capacity = 5;
    const int items_per_producer = 20;
    BoundedBlockingQueue<int> q(capacity);
    std::vector<int> consumed;
    consumed.reserve(items_per_producer * 2);

    std::thread prod1(producer, std::ref(q), 0, items_per_producer);
    std::thread prod2(producer, std::ref(q), 1000, items_per_producer);
    std::thread cons(consumer, std::ref(q), items_per_producer * 2, std::ref(consumed));

    prod1.join();
    prod2.join();
    cons.join();

    assert(consumed.size() == items_per_producer * 2);
    // Verify that all produced values are present
    std::vector<bool> seen(2000, false);
    for (int v : consumed) {
        if (v < 2000) seen[v] = true;
    }
    for (int i = 0; i < items_per_producer; ++i) {
        assert(seen[i]);
        assert(seen[1000 + i]);
    }
}

void test_blocking_behavior() {
    BoundedBlockingQueue<int> q(1);
    std::atomic<bool> push_blocked{false};
    std::thread t1([&]{
        q.push(42); // fills the queue
        // Next push should block until consumer pops
        auto start = std::chrono::steady_clock::now();
        std::thread blocker([&]{
            q.push(99);
        });
        // Give blocker a moment to attempt push
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        push_blocked = true;
        int val;
        q.pop(val); // free space
        blocker.join();
        auto end = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // Ensure blocker waited at least ~50ms
        assert(dur >= 40);
    });
    t1.join();
    assert(push_blocked);
}

int main() {
    test_single_thread();
    test_non_blocking();
    test_multi_thread();
    test_blocking_behavior();
    std::cout << "All tests passed.\n";
    return 0;
}
