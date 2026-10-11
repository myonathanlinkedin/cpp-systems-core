#include "types.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>

int main() {
    // Single-threaded test
    {
        BoundedBlockingQueue<int> q(3);
        assert(q.size() == 0);
        q.push(1);
        q.push(2);
        assert(q.size() == 2);
        int a = q.pop();
        int b = q.pop();
        assert(a == 1 && b == 2);
        assert(q.size() == 0);
    }

    // Multi-threaded test: producer-consumer
    {
        BoundedBlockingQueue<int> q(2);
        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};
        std::thread producer([&]() {
            for (int i = 0; i < 5; ++i) {
                q.push(i);
                ++produced;
            }
        });
        std::thread consumer([&]() {
            for (int i = 0; i < 5; ++i) {
                int val = q.pop();
                assert(val == i);
                ++consumed;
            }
        });
        producer.join();
        consumer.join();
        assert(produced == 5);
        assert(consumed == 5);
    }

    // Test blocking on full
    {
        BoundedBlockingQueue<int> q(1);
        std::atomic<bool> second_push_done{false};
        std::thread producer([&]() {
            q.push(10); // fills queue
            // second push should block until consumer pops
            q.push(20);
            second_push_done = true;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        assert(!second_push_done.load()); // should still be blocked
        int val = q.pop(); // consumer pops first item
        assert(val == 10);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        assert(second_push_done.load()); // now second push completed
        int val2 = q.pop();
        assert(val2 == 20);
        producer.join();
    }

    // Test blocking on empty with try_pop timeout
    {
        BoundedBlockingQueue<int> q(2);
        int out;
        bool res = q.try_pop(out, std::chrono::milliseconds(200));
        assert(!res); // queue empty, should timeout
        q.push(42);
        res = q.try_pop(out, std::chrono::milliseconds(200));
        assert(res && out == 42);
    }

    std::cout << "All tests passed.\n";
    return 0;
}
