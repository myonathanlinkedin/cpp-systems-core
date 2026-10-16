#include <cmath>

#include "types.hpp"
#include "core.hpp"

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

using namespace hll;

namespace {

constexpr double REL_TOL = 0.07; // 7 % relative tolerance for estimates

bool approxEqual(double a, double b, double relTol = REL_TOL) {
    if (a == 0.0 && b == 0.0) return true;
    return std::abs(a - b) <= relTol * std::max(std::abs(a), std::abs(b));
}

// Simple deterministic pseudo‑random generator for reproducible tests.
uint64_t deterministicHash(uint64_t x) {
    // SplitMix64 (same as used in core)
    x += 0x9e3779b97f4a7c15ULL;
    uint64_t z = x;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return z;
}

void test_small_cardinality() {
    HyperLogLog hll(10); // m = 1024
    for (int i = 0; i < 10; ++i) {
        hll.add(i);
    }
    double est = hll.estimate();
    assert(approxEqual(est, 10.0));
}

void test_medium_cardinality() {
    HyperLogLog hll(12); // m = 4096
    const int N = 50000;
    for (int i = 0; i < N; ++i) {
        hll.add(i);
    }
    double est = hll.estimate();
    assert(approxEqual(est, static_cast<double>(N)));
}

void test_large_cardinality() {
    HyperLogLog hll(14); // m = 16384
    const int N = 2000000;
    for (int i = 0; i < N; ++i) {
        hll.add(i);
    }
    double est = hll.estimate();
    assert(approxEqual(est, static_cast<double>(N)));
}

void test_merge() {
    HyperLogLog h1(12), h2(12);
    const int N1 = 30000;
    const int N2 = 40000;
    for (int i = 0; i < N1; ++i) h1.add(i);
    for (int i = N1; i < N1 + N2; ++i) h2.add(i);
    HyperLogLog merged = h1;
    merged.merge(h2);
    double est = merged.estimate();
    assert(approxEqual(est, static_cast<double>(N1 + N2)));
}

void test_deterministic_hash_consistency() {
    HyperLogLog hll(12);
    for (uint64_t i = 0; i < 1000; ++i) {
        hll.addHash(deterministicHash(i));
    }
    double est = hll.estimate();
    assert(approxEqual(est, 1000.0));
}

void run_all_tests() {
    test_small_cardinality();
    test_medium_cardinality();
    test_large_cardinality();
    test_merge();
    test_deterministic_hash_consistency();
    std::cout << "All HyperLogLog tests passed.\n";
}

} // anonymous namespace

int main() {
    run_all_tests();
    return 0;
}
