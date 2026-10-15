#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace hll {

class HyperLogLog {
public:
    // Construct with precision p (4 <= p <= 16). m = 2^p registers.
    explicit HyperLogLog(uint8_t precision = 12);

    // Add an element using a 64‑bit hash value.
    void addHash(uint64_t hash);

    // Add an element of arbitrary type (uses std::hash).
    template <typename T>
    void add(const T& element);

    // Estimate the cardinality.
    double estimate() const;

    // Merge another HLL into this one (register‑wise max).
    void merge(const HyperLogLog& other);

    // Accessors for testing.
    uint8_t precision() const { return p_; };
    std::size_t registerCount() const { return registers_.size(); }
    const std::vector<uint8_t>& registers() const { return registers_; }

private:
    uint8_t p_;                     // precision
    std::size_t m_;                 // number of registers = 2^p
    std::vector<uint8_t> registers_; // registers storing max rank

    static constexpr double getAlpha(uint8_t p);
    static uint8_t rho(uint64_t w, uint8_t maxBits);
};

} // namespace hll
