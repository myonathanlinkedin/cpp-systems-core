#include "types.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace mempool;

int main()
{
    // Create a pool of 64‑byte blocks, 1024 blocks total.
    MemoryPool<64, 1024> pool;

    // ---------- Test 1: Exhaustion ----------
    std::vector<void*> allocated;
    allocated.reserve(pool.capacity());

    for (std::size_t i = 0; i < pool.capacity(); ++i) {
        void* p = pool.allocate();
        assert(p != nullptr && "Allocation should succeed while pool not exhausted.");
        // Ensure each pointer is unique.
        for (void* prev : allocated) {
            assert(prev != p && "Allocated pointer must be unique.");
        }
        allocated.push_back(p);
    }

    // Pool should now be empty.
    assert(pool.allocate() == nullptr && "Allocation must fail when pool is exhausted.");
    assert(pool.used() == pool.capacity() && "All blocks should be reported as used.");

    // ---------- Test 2: Deallocation and reuse ----------
    // Deallocate half of the blocks.
    for (std::size_t i = 0; i < allocated.size(); i += 2) {
        pool.deallocate(allocated[i]);
    }
    std::size_t used_after_dealloc = pool.used();
    assert(used_after_dealloc == pool.capacity() / 2 && "Half the blocks should be free.");

    // Allocate again; we should get exactly the number of freed blocks.
    std::size_t reallocated = 0;
    for (std::size_t i = 0; i < pool.capacity() / 2; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr && "Re‑allocation should succeed for freed blocks.");
        ++reallocated;
    }
    assert(reallocated == pool.capacity() / 2);
    assert(pool.used() == pool.capacity() && "All blocks should be used again.");

    // ---------- Test 3: Reset ----------
    pool.reset();
    assert(pool.used() == 0 && "After reset, no blocks should be in use.");
    // After reset we can allocate the full capacity again.
    for (std::size_t i = 0; i < pool.capacity(); ++i) {
        void* p = pool.allocate();
        assert(p != nullptr);
    }
    assert(pool.allocate() == nullptr && "Pool must be exhausted after full re‑allocation.");

    // ---------- Test 4: Edge cases ----------
    // Deallocate nullptr – should be a no‑op and not corrupt the pool.
    pool.deallocate(nullptr);
    // Deallocate a pointer not belonging to the pool – should trigger an assert in debug builds.
    // Uncomment the following line to see the assertion (only in debug builds):
    // pool.deallocate(reinterpret_cast<void*>(0xDEADBEEF));

    std::cout << "All MemoryPool unit tests passed successfully.\n";
    return 0;
}
