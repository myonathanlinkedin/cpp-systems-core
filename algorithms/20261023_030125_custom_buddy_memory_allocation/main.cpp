#include <iostream>
#include <vector>
#include <cassert>
#include "core.cpp"

int main() {
    using buddy::BuddyAllocator;

    // Basic parameters
    constexpr std::size_t TOTAL_SIZE = 1 << 16; // 64 KiB
    constexpr std::size_t MIN_BLOCK = 32;      // 32 bytes

    BuddyAllocator allocator(TOTAL_SIZE, MIN_BLOCK);

    // Test 1: allocate whole buffer
    void* p1 = allocator.allocate(TOTAL_SIZE - sizeof(buddy::Header));
    assert(p1 != nullptr);
    assert(allocator.totalFreeMemory() == 0);
    allocator.deallocate(p1);
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    // Test 2: allocate several small blocks and ensure no overlap
    std::vector<void*> ptrs;
    for (int i = 0; i < 100; ++i) {
        void* p = allocator.allocate(100); // 100 bytes each
        assert(p != nullptr);
        ptrs.push_back(p);
    }
    // All allocations should have reduced free memory
    std::size_t used = 100 * (100 + sizeof(buddy::Header));
    assert(allocator.totalFreeMemory() + used <= TOTAL_SIZE);

    // Deallocate in reverse order
    for (auto it = ptrs.rbegin(); it != ptrs.rend(); ++it) {
        allocator.deallocate(*it);
    }
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    // Test 3: allocate maximum possible block (exact power of two)
    void* p2 = allocator.allocate(TOTAL_SIZE / 2 - sizeof(buddy::Header));
    assert(p2 != nullptr);
    allocator.deallocate(p2);
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    // Test 4: request too large block
    void* p3 = allocator.allocate(TOTAL_SIZE); // exceeds after header
    assert(p3 == nullptr);
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    // Test 5: random sized allocations and immediate deallocations
    for (int i = 0; i < 1000; ++i) {
        std::size_t sz = (i % 128) + 1;
        void* p = allocator.allocate(sz);
        assert(p != nullptr);
        allocator.deallocate(p);
    }
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    // Test 6: allocate, deallocate, allocate again to check reuse
    void* a = allocator.allocate(200);
    void* b = allocator.allocate(300);
    assert(a && b);
    allocator.deallocate(a);
    allocator.deallocate(b);
    void* c = allocator.allocate(250);
    assert(c != nullptr);
    allocator.deallocate(c);
    assert(allocator.totalFreeMemory() == TOTAL_SIZE);

    std::cout << "All BuddyAllocator tests passed.\n";
    return 0;
}
