#include <iostream>
#include <cassert>
#include <cmath>

#include "types.hpp"
#include "engine.cpp"   // bring in implementation (as per specification)

int main() {
    using namespace hll;

    // ---------- Test 1: Empty sketch ----------
    {
        HyperLogLog hll(10); // 1024 registers
        double est = hll.estimate();
        assert(est == 0.0 && "Empty sketch should estimate zero");
    }

    // ---------- Test 2: Single element ----------
    {
        HyperLogLog hll(10);
        hll.add("unique_element");
        double est = hll.estimate();
        // Estimate may be slightly >1 due to bias; accept tolerance 0.5
        assert(std::abs(est - 1.0) <= 0.5 && "Single element estimate should be close to 1");
    }

    // ---------- Test 3: Known cardinality ----------
    {
        const std::size_t N = 10000;
        HyperLogLog hll(12); // 4096 registers, good accuracy for 10k
        for (std::size_t i = 0; i < N; ++i) {
            hll.add("elem_" + std::to_string(i));
        }
        double est = hll.estimate();
        double rel_error = std::abs(est - static_cast<double>(N)) / static_cast<double>(N);
        // HyperLogLog theoretical error ≈ 1.04 / sqrt(m)
        double theoretical = 1.04 / std::sqrt(static_cast<double>(1U << 12));
        // Allow 3× theoretical error as generous bound
        assert(rel_error <= 3.0 * theoretical && "Estimate error exceeds acceptable bound");
    }

    // ---------- Test 4: Merge two sketches ----------
    {
        const std::size_t N = 5000;
        HyperLogLog hllA(12);
        HyperLogLog hllB(12);
        for (std::size_t i = 0; i < N; ++i) {
            hllA.add("a_" + std::to_string(i));
            hllB.add("b_" + std::to_string(i));
        }
        // Merge B into A
        hllA.merge(hllB);
        double est = hllA.estimate();
        double expected = static_cast<double>(2 * N);
        double rel_error = std::abs(est - expected) / expected;
        double theoretical = 1.04 / std::sqrt(static_cast<double>(1U << 12));
        assert(rel_error <= 3.0 * theoretical && "Merged estimate error exceeds acceptable bound");
    }

    // ---------- Test 5: Consistency across repeated adds ----------
    {
        HyperLogLog hll(12);
        hll.add("duplicate");
        hll.add("duplicate");
        hll.add("duplicate");
        double est = hll.estimate();
        assert(std::abs(est - 1.0) <= 0.5 && "Duplicate inserts should not inflate estimate");
    }

    // Demo output
    {
        HyperLogLog demo(14); // 16384 registers for higher accuracy
        const std::size_t demoN = 123456;
        for (std::size_t i = 0; i < demoN; ++i) {
            demo.add("demo_" + std::to_string(i));
        }
        std::cout << "Demo: inserted " << demoN << " distinct elements.\n";
        std::cout << "Estimated cardinality: " << static_cast<std::uint64_t>(demo.estimate()) << "\n";
    }

    std::cout << "All HyperLogLog unit tests passed.\n";
    return 0;
}
