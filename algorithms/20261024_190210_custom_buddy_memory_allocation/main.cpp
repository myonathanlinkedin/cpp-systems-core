#include <cassert>

#include "types.hpp"
#include <iostream>

int main() {
    using namespace buddy;

    // Create an allocator with block sizes from 2^4 (16) to 2^10 (1024) bytes.
    BuddyAllocator allocator(4, 10);

    // Basic sanity checks.
    assert(allocator.total_size() == 1024);
    assert(allocator.free_count(10) == 1);
    for (std::size_t o = 4; o < 10; ++o) {
        assert(allocator.free_count(o) == 0);
    }

    // Allocate a small block.
    void* p1 = allocator.allocate(20); // Needs at least 32 bytes block.
    assert(p1 != nullptr);
    assert(allocator.free_count(10) == 0);
    assert(allocator.free_count(5) == 1); // One 32‑byte block split from 1024.

    // Allocate another block.
    void* p2 = allocator.allocate(50); // Needs 64‑byte block.
    assert(p2 != nullptr);
    assert(allocator.free_count(6) == 1); // One 64‑byte block left from split.

    // Deallocate first block and ensure it coalesces correctly.
    allocator.deallocate(p1);
    // After freeing a 32‑byte block, it should not coalesce yet because its buddy is allocated.
    assert(allocator.free_count(5) == 1);

    // Deallocate second block.
    allocator.deallocate(p2);
    // Now the two 32‑byte buddies should coalesce up to a 64‑byte free block.
    assert(allocator.free_count(6) == 1);
    // The 64‑byte block can further coalesce with its buddy (still free) up to 128‑byte, etc.
    // Since we only split once, we expect a single 64‑byte free block at this point.
    // Allocate a large block that uses the whole pool.
    void* p3 = allocator.allocate(800); // Needs 1024‑byte block.
    assert(p3 != nullptr);
    // All memory is now allocated.
    for (std::size_t o = 4; o <= 10; ++o) {
        assert(allocator.free_count(o) == 0);
    }

    // Free the large block and ensure the allocator returns to initial state.
    allocator.deallocate(p3);
    assert(allocator.free_count(10) == 1);
    for (std::size_t o = 4; o < 10; ++o) {
        assert(allocator.free_count(o) == 0);
    }

    // Edge case: request size larger than pool.
    void* p4 = allocator.allocate(2048);
    assert(p4 == nullptr);

    // Edge case: zero-size allocation.
    void* p5 = allocator.allocate(0);
    assert(p5 == nullptr);

    // Stress test: allocate and free many small blocks.
    constexpr std::size_t N = 50;
    void* ptrs[N];
    for (std::size_t i = 0; i < N; ++i) {
        ptrs[i] = allocator.allocate(8); // 8 bytes -> needs 16‑byte block.
        assert(ptrs[i] != nullptr);
    }
    // All 16‑byte blocks should be exhausted.
    assert(allocator.free_count(4) == 0);
    // Free them in reverse order.
    for (std::size_t i = N; i-- > 0;) {
        allocator.deallocate(ptrs[i]);
    }
    // After full deallocation, the allocator should have coalesced back to a single max block.
    assert(allocator.free_count(10) == 1);
    for (std::size_t o = 4; o < 10; ++o) {
        assert(allocator.free_count(o) == 0);
    }

    std::cout << "All buddy allocator tests passed.\n";
    return 0;
}
