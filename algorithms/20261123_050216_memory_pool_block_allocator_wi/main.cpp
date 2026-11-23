#include <algorithm>

#include <iostream>
#include <vector>
#include <cassert>
#include "core.cpp"   // Pull in the MemoryPool definition.

int main() {
    using mempool::MemoryPool;

    // Test parameters.
    constexpr std::size_t blockSize = 32;
    constexpr std::size_t blockCount = 10;

    MemoryPool pool(blockSize, blockCount);
    assert(pool.capacity() == blockCount);
    assert(pool.used() == 0);
    assert(pool.free() == blockCount);

    // Allocate all blocks.
    std::vector<void*> allocated;
    for (std::size_t i = 0; i < blockCount; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr);
        allocated.push_back(p);
        // Write a pattern to verify memory is usable.
        std::fill_n(static_cast<char*>(p), blockSize, static_cast<char>(i));
    }
    assert(pool.used() == blockCount);
    assert(pool.free() == 0);

    // Pool should now be exhausted.
    assert(pool.allocate() == nullptr);

    // Deallocate half of the blocks in reverse order.
    for (std::size_t i = 0; i < blockCount / 2; ++i) {
        void* p = allocated.back();
        allocated.pop_back();
        pool.deallocate(p);
    }
    assert(pool.used() == blockCount - blockCount / 2);
    assert(pool.free() == blockCount / 2);

    // Allocate again; should succeed for the freed slots.
    for (std::size_t i = 0; i < blockCount / 2; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr);
        // Verify that the pattern we wrote earlier is still present (optional).
        // Not required for allocator correctness.
    }
    assert(pool.used() == blockCount);
    assert(pool.free() == 0);

    // Clean up: deallocate everything.
    while (!allocated.empty()) {
        pool.deallocate(allocated.back());
        allocated.pop_back();
    }
    // Deallocate the blocks we just re‑allocated.
    for (std::size_t i = 0; i < blockCount; ++i) {
        void* p = pool.allocate(); // should succeed because we just freed all.
        assert(p != nullptr);
        pool.deallocate(p);
    }
    assert(pool.used() == 0);
    assert(pool.free() == blockCount);

    // Move semantics test.
    MemoryPool pool2(std::move(pool));
    assert(pool2.capacity() == blockCount);
    assert(pool2.used() == 0);
    assert(pool2.free() == blockCount);
    // Original pool should be empty after move.
    assert(pool.capacity() == 0);
    assert(pool.used() == 0);
    assert(pool.free() == 0);

    std::cout << "All MemoryPool tests passed.\n";
    return 0;
}
