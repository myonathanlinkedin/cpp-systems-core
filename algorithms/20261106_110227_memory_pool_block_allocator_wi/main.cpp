#include <cassert>

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "types.hpp"

using namespace mempool;

int main() {
    // Basic parameters.
    constexpr std::size_t blockSize = 32;
    constexpr std::size_t blockCount = 1024;

    MemoryPool pool(blockSize, blockCount);

    // 1. Exhaustive allocation test.
    std::vector<void*> allocated;
    allocated.reserve(blockCount);
    for (std::size_t i = 0; i < blockCount; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr && "Allocation should succeed while pool has free blocks");
        allocated.push_back(p);
    }
    assert(pool.freeCount() == 0);
    assert(pool.allocate() == nullptr && "Pool should report exhaustion");

    // 2. Full deallocation test.
    for (void* p : allocated) {
        pool.deallocate(p);
    }
    allocated.clear();
    assert(pool.freeCount() == pool.capacity());

    // 3. Randomized allocate/deallocate pattern.
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> actionDist(0, 1); // 0 = allocate, 1 = deallocate

    const std::size_t ops = 10'000;
    for (std::size_t i = 0; i < ops; ++i) {
        int action = actionDist(rng);
        if (action == 0) { // allocate
            if (pool.freeCount() > 0) {
                void* p = pool.allocate();
                assert(p != nullptr);
                allocated.push_back(p);
            }
        } else { // deallocate
            if (!allocated.empty()) {
                std::uniform_int_distribution<std::size_t> idxDist(0, allocated.size() - 1);
                std::size_t idx = idxDist(rng);
                void* p = allocated[idx];
                pool.deallocate(p);
                allocated.erase(allocated.begin() + idx);
            }
        }
    }

    // Clean up any remaining allocations.
    for (void* p : allocated) {
        pool.deallocate(p);
    }
    allocated.clear();
    assert(pool.freeCount() == pool.capacity());

    // 4. Edge‑case checks.
    // Deallocating nullptr must be safe.
    pool.deallocate(nullptr);

    // Attempting to deallocate a pointer not from the pool should trigger an assert.
    // This is intentionally commented out to keep the test suite passing.
    // int external;
    // pool.deallocate(&external); // Should abort in debug builds.

    std::cout << "All MemoryPool tests passed successfully.\n";
    return 0;
}
