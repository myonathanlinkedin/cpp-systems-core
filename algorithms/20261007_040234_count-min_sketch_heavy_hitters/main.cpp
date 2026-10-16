#include <algorithm>

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <random>
#include "types.hpp"
#include "core.hpp"

int main() {
    // Test 1: Basic add and estimate
    CountMinSketch cms1(0.01, 0.01);
    cms1.add("a", 10);
    cms1.add("b", 5);
    uint64_t estA = cms1.estimate("a");
    uint64_t estB = cms1.estimate("b");
    uint64_t estC = cms1.estimate("c"); // unseen
    assert(estA >= 10);
    assert(estB >= 5);
    assert(estC == 0);

    // Test 2: Heavy hitters detection
    CountMinSketch cms2(0.01, 0.01);
    for (int i = 0; i < 100; ++i) cms2.add("a");
    for (int i = 0; i < 50; ++i) cms2.add("b");
    auto hitters2 = cms2.heavyHitters(0.5); // threshold 50%
    assert(hitters2.size() == 1);
    assert(hitters2[0] == "a");

    // Test 3: Empty sketch
    CountMinSketch cms3(0.01, 0.01);
    auto hitters3 = cms3.heavyHitters(0.1);
    assert(hitters3.empty());

    // Test 4: Large counts
    CountMinSketch cms4(0.01, 0.01);
    for (int i = 0; i < 1000; ++i) cms4.add("x");
    assert(cms4.estimate("x") >= 1000);

    // Test 5: Random stream with heavy hitters
    CountMinSketch cms5(0.01, 0.01);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 99);
    // Add 10 heavy keys each 100 times
    std::vector<std::string> heavyKeys;
    for (int i = 0; i < 10; ++i) {
        std::string key = "heavy_" + std::to_string(i);
        heavyKeys.push_back(key);
        for (int j = 0; j < 100; ++j) cms5.add(key);
    }
    // Add random keys
    for (int i = 0; i < 1000; ++i) {
        int r = dist(rng);
        cms5.add("rand_" + std::to_string(r));
    }
    auto hitters5 = cms5.heavyHitters(0.05); // 5% threshold
    // All heavy keys should be present
    for (const auto& key : heavyKeys) {
        assert(std::find(hitters5.begin(), hitters5.end(), key) != hitters5.end());
    }
    // Number of heavy hitters should be at least 10
    assert(hitters5.size() >= 10);

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
