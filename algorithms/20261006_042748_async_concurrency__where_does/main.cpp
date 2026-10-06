#include <iostream>

#include "types.hpp"
#include "core.hpp"
#include <cassert>
#include <chrono>

int main() {
    // Test 1: Basic task execution and counter increment.
    {
        Scheduler scheduler(4);
        Counter counter;
        const int task_count = 1000;
        for (int i = 0; i < task_count; ++i) {
            scheduler.submit([&counter] { counter.increment(); });
        }
        scheduler.shutdown();
        assert(counter.value.load() == task_count);
        std::cout << "Test 1 passed: counter = " << counter.value.load() << std::endl;
    }

    // Test 2: Submit after shutdown should throw.
    {
        Scheduler scheduler(2);
        scheduler.shutdown();
        bool caught = false;
        try {
            scheduler.submit([]{});
        } catch (const std::runtime_error&) {
            caught = true;
        }
        assert(caught && "Submitting after shutdown did not throw");
        std::cout << "Test 2 passed: exception thrown on submit after shutdown." << std::endl;
    }

    // Test 3: Scheduler with zero threads should assert.
    {
        bool caught = false;
        try {
            Scheduler scheduler(0);
        } catch (const std::exception&) {
            caught = true;
        }
        assert(caught && "Scheduler did not assert on zero threads");
        std::cout << "Test 3 passed: assertion on zero threads." << std::endl;
    }

    // Test 4: Stress test with many tasks and threads.
    {
        const std::size_t thread_count = 8;
        const int task_count = 5000;
        Scheduler scheduler(thread_count);
        Counter counter;
        for (int i = 0; i < task_count; ++i) {
            scheduler.submit([&counter] { counter.increment(); });
        }
        scheduler.shutdown();
        assert(counter.value.load() == task_count);
        std::cout << "Test 4 passed: stress test counter = " << counter.value.load() << std::endl;
    }

    // Test 5: Ensure scheduler can be reused by creating a new instance.
    {
        Scheduler scheduler(3);
        Counter counter;
        scheduler.submit([&counter] { counter.increment(); });
        scheduler.shutdown();
        assert(counter.value.load() == 1);
        std::cout << "Test 5 passed: scheduler reused successfully." << std::endl;
    }

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
