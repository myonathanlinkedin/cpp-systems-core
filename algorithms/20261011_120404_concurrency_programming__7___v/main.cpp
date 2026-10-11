#include <chrono>

#include "types.hpp"
#include <iostream>
#include <thread>
#include <cassert>

int main() {
    using namespace concurrency;

    // -----------------------------------------------------------------
    // Single‑threaded sanity checks
    // -----------------------------------------------------------------
    {
        VolatileFlag vf;
        assert(!vf.is_set());   // initially false
        vf.set();
        assert(vf.is_set());    // after set() it must be true
        vf.clear();
        assert(!vf.is_set());   // after clear() it must be false
    }

    // -----------------------------------------------------------------
    // Multi‑threaded wait_until_set test
    // -----------------------------------------------------------------
    {
        VolatileFlag vf;
        std::thread setter([&vf]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            vf.set();
        });

        auto start = std::chrono::steady_clock::now();
        vf.wait_until_set(std::chrono::milliseconds(500));
        auto elapsed = std::chrono::steady_clock::now() - start;

        assert(vf.is_set());    // the flag must be true after the setter runs
        // The wait should have taken roughly the sleep time of the setter.
        assert(elapsed >= std::chrono::milliseconds(90));
        assert(elapsed < std::chrono::milliseconds(500));

        setter.join();
    }

    // -----------------------------------------------------------------
    // Timeout behavior when the flag never becomes true
    // -----------------------------------------------------------------
    {
        VolatileFlag vf;
        auto start = std::chrono::steady_clock::now();
        vf.wait_until_set(std::chrono::milliseconds(200));
        auto elapsed = std::chrono::steady_clock::now() - start;

        assert(!vf.is_set());   // still false
        // The elapsed time should be close to the timeout value.
        assert(elapsed >= std::chrono::milliseconds(190));
    }

    std::cout << "All tests passed.\n";
    return 0;
}
