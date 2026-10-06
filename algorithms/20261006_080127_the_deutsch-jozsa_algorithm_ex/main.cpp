#include <vector>

#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>

using namespace dj;

int main() {
    // Test 1: Constant zero function
    auto constZero = [](BitString) { return 0; };
    assert(deutschJozsa(constZero, 3) == true);
    std::cout << "Test 1 passed (constant zero).\n";

    // Test 2: Constant one function
    auto constOne = [](BitString) { return 1; };
    assert(deutschJozsa(constOne, 4) == true);
    std::cout << "Test 2 passed (constant one).\n";

    // Test 3: Balanced function (parity)
    auto parity = [](BitString x) { return static_cast<int>(__builtin_popcountll(x) & 1); };
    assert(deutschJozsa(parity, 5) == false);
    std::cout << "Test 3 passed (balanced parity).\n";

    // Test 4: Random balanced function generator
    std::mt19937_64 rng(42);
    for (std::size_t n = 1; n <= 6; ++n) {
        std::size_t size = 1ULL << n;
        std::vector<int> table(size);
        // Fill half with 0, half with 1, then shuffle
        for (std::size_t i = 0; i < size / 2; ++i) table[i] = 0;
        for (std::size_t i = size / 2; i < size; ++i) table[i] = 1;
        std::shuffle(table.begin(), table.end(), rng);
        auto balanced = [&](BitString x) { return table[static_cast<std::size_t>(x)]; };
        assert(deutschJozsa(balanced, n) == false);
    }
    std::cout << "Test 4 passed (random balanced functions).\n";

    // Benchmark
    const std::size_t bench_n = 10;
    auto bench_func = [&](BitString x) { return static_cast<int>(x & 1ULL); }; // balanced
    auto start = std::chrono::high_resolution_clock::now();
    bool result = deutschJozsa(bench_func, bench_n);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Benchmark n=" << bench_n << " result=" << (result ? "constant" : "balanced")
              << " time=" << elapsed.count() << " s\n";

    std::cout << "All tests passed.\n";
    return 0;
}
