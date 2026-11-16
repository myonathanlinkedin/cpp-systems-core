#include <cassert>
#include <iostream>
#include "core.hpp"

int main() {
    using lsm::LRUCache;

    // Test 1: Basic put/get.
    {
        LRUCache cache(2);
        cache.put(1, 10);
        cache.put(2, 20);
        assert(cache.get(1) == 10); // Access moves key 1 to MRU.
        cache.put(3, 30);           // Evicts key 2.
        assert(cache.get(2) == -1);
        assert(cache.get(3) == 30);
        assert(cache.get(1) == 10);
    }

    // Test 2: Update existing key.
    {
        LRUCache cache(2);
        cache.put(1, 100);
        cache.put(1, 101); // Update value, should stay MRU.
        assert(cache.get(1) == 101);
        cache.put(2, 200);
        cache.put(3, 300); // Evicts key 2 (LRU).
        assert(cache.get(2) == -1);
        assert(cache.get(3) == 300);
        assert(cache.get(1) == 101);
    }

    // Test 3: Capacity zero (cache disabled).
    {
        LRUCache cache(0);
        cache.put(1, 1);
        assert(cache.get(1) == -1);
        cache.put(2, 2);
        assert(cache.get(2) == -1);
    }

    // Test 4: Stress test with many operations.
    {
        const std::size_t cap = 1000;
        LRUCache cache(cap);
        // Fill cache.
        for (int i = 0; i < static_cast<int>(cap); ++i) {
            cache.put(i, i * 10);
        }
        // Access a subset to make them MRU.
        for (int i = 0; i < 500; ++i) {
            assert(cache.get(i) == i * 10);
        }
        // Insert additional items to trigger evictions.
        for (int i = static_cast<int>(cap); i < static_cast<int>(cap) + 500; ++i) {
            cache.put(i, i * 10);
        }
        // Keys 500..999 should have been evicted.
        for (int i = 500; i < static_cast<int>(cap); ++i) {
            assert(cache.get(i) == -1);
        }
        // Keys 0..499 should still be present.
        for (int i = 0; i < 500; ++i) {
            assert(cache.get(i) == i * 10);
        }
        // Newly added keys should be present.
        for (int i = static_cast<int>(cap); i < static_cast<int>(cap) + 500; ++i) {
            assert(cache.get(i) == i * 10);
        }
    }

    std::cout << "All LRUCache unit tests passed.\n";
    return 0;
}
