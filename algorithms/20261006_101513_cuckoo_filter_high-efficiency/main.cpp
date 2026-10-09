#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

int main() {
    CuckooFilterConfig cfg;
    cfg.bucket_count = 1 << 12; // 4096 buckets
    cfg.bucket_size = 4;
    cfg.max_kicks = 200;
    CuckooFilter filter(cfg);

    // Basic insert / lookup / delete test
    std::vector<uint64_t> keys;
    for (uint64_t i = 1; i <= 1000; ++i) {
        keys.push_back(i * 1234567ULL);
    }

    // Insert all keys
    for (auto k : keys) {
        bool ok = filter.insert(k);
        assert(ok && "Insertion should succeed");
    }
    assert(filter.size() == keys.size());

    // Verify presence
    for (auto k : keys) {
        assert(filter.contains(k) && "Key must be present after insertion");
    }

    // Delete half of them
    for (std::size_t i = 0; i < keys.size(); i += 2) {
        bool removed = filter.erase(keys[i]);
        assert(removed && "Deletion should succeed");
    }

    // Verify deletions and remaining presence
    for (std::size_t i = 0; i < keys.size(); ++i) {
        bool present = filter.contains(keys[i]);
        if (i % 2 == 0) {
            assert(!present && "Even-indexed key should have been removed");
        } else {
            assert(present && "Odd-indexed key should still be present");
        }
    }

    // False positive check
    std::size_t false_positives = 0;
    std::size_t trials = 5000;
    for (uint64_t i = 0; i < trials; ++i) {
        uint64_t probe = (i + 999999ULL) * 7654321ULL;
        if (filter.contains(probe) && std::find(keys.begin(), keys.end(), probe) == keys.end())
            ++false_positives;
    }
    double fp_rate = static_cast<double>(false_positives) / trials;
    std::cout << "False positive rate: " << fp_rate << std::endl;
    // Expect low false positive rate (<0.02) for this configuration
    assert(fp_rate < 0.05 && "False positive rate too high");

    // Stress test: fill to capacity (may stop early if filter reports full)
    std::size_t inserted = 0;
    for (uint64_t i = 1000000; i < 2000000; ++i) {
        if (filter.insert(i)) ++inserted;
        else break;
    }
    std::cout << "Inserted additional " << inserted << " items before filter reported full.\n";

    // Ensure size never exceeds capacity
    assert(filter.size() <= filter.capacity());

    std::cout << "All tests passed.\n";
    return 0;
}
