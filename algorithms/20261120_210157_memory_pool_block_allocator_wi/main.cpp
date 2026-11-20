#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cassert>
#include "core.cpp"

int main() {
    constexpr std::size_t BLOCK_SIZE = 32;
    constexpr std::size_t BLOCK_COUNT = 1024;

    MemoryPool pool(BLOCK_SIZE, BLOCK_COUNT);
    assert(pool.capacity() == BLOCK_COUNT);
    assert(pool.used() == 0);
    assert(pool.free() == BLOCK_COUNT);

    // ---------- Test 1: allocate all blocks ----------
    std::vector<void*> ptrs;
    ptrs.reserve(BLOCK_COUNT);
    for (std::size_t i = 0; i < BLOCK_COUNT; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr);
        assert(pool.owns(p));
        ptrs.push_back(p);
    }
    assert(pool.used() == BLOCK_COUNT);
    assert(pool.free() == 0);
    // extra allocation must fail
    assert(pool.allocate() == nullptr);

    // ---------- Test 2: deallocate in reverse order ----------
    for (auto it = ptrs.rbegin(); it != ptrs.rend(); ++it) {
        pool.deallocate(*it);
    }
    assert(pool.used() == 0);
    assert(pool.free() == BLOCK_COUNT);

    // ---------- Test 3: random deallocation ----------
    ptrs.clear();
    for (std::size_t i = 0; i < BLOCK_COUNT; ++i) {
        ptrs.push_back(pool.allocate());
    }
    std::shuffle(ptrs.begin(), ptrs.end(), std::mt19937{std::random_device{}()});
    for (void* p : ptrs) {
        pool.deallocate(p);
    }
    assert(pool.used() == 0);
    assert(pool.free() == BLOCK_COUNT);

    // ---------- Test 4: reuse after full deallocation ----------
    for (std::size_t i = 0; i < BLOCK_COUNT; ++i) {
        void* p = pool.allocate();
        assert(p != nullptr);
        // write a pattern to ensure the memory is usable
        std::fill_n(static_cast<std::uint8_t*>(p), BLOCK_SIZE, static_cast<std::uint8_t>(i & 0xFF));
    }
    assert(pool.used() == BLOCK_COUNT);
    // clean up
    while (void* p = pool.allocate()) {
        pool.deallocate(p);
    }

    // ---------- Test 5: owns() edge cases ----------
    int dummy;
    assert(!pool.owns(&dummy)); // external pointer
    assert(!pool.owns(nullptr)); // null pointer

    std::cout << "All MemoryPool tests passed.\n";
    return 0;
}
