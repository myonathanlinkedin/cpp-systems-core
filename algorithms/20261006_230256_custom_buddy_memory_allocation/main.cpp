#include "types.hpp"
#include "core.hpp"
#include <cassert>
#include <iostream>

int main() {
    // Create a buddy allocator with total size 2^12 = 4096 bytes
    BuddyAllocator allocator(12);

    // Simple allocation and deallocation
    void* a = allocator.allocate(100);
    assert(a != nullptr);
    void* b = allocator.allocate(200);
    assert(b != nullptr);
    allocator.deallocate(a);
    allocator.deallocate(b);

    // Allocate a block that exactly fits the whole memory (minus header)
    void* whole = allocator.allocate(allocator.total_size() - sizeof(BlockHeader));
    assert(whole != nullptr);
    allocator.deallocate(whole);

    // Stress test: allocate many small blocks, then free them in reverse order
    const int N = 30;
    void* ptrs[N];
    for (int i = 0; i < N; ++i) {
        ptrs[i] = allocator.allocate(64);
        assert(ptrs[i] != nullptr);
    }
    for (int i = N - 1; i >= 0; --i) {
        allocator.deallocate(ptrs[i]);
    }

    // Allocate varying sizes and ensure they are distinct
    void* p1 = allocator.allocate(50);
    void* p2 = allocator.allocate(500);
    void* p3 = allocator.allocate(1024);
    assert(p1 != nullptr && p2 != nullptr && p3 != nullptr);
    assert(p1 != p2 && p2 != p3 && p1 != p3);
    allocator.deallocate(p2);
    allocator.deallocate(p1);
    allocator.deallocate(p3);

    // Allocate after fragmentation to test coalescing
    void* f1 = allocator.allocate(200);
    void* f2 = allocator.allocate(200);
    void* f3 = allocator.allocate(200);
    assert(f1 && f2 && f3);
    allocator.deallocate(f2);
    allocator.deallocate(f1);
    // Now a larger block should be possible
    void* large = allocator.allocate(500);
    assert(large != nullptr);
    allocator.deallocate(f3);
    allocator.deallocate(large);

    std::cout << "All buddy allocator tests passed.\n";
    return 0;
}
