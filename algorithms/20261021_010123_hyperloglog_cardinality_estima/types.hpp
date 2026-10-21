#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <limits>

namespace hll {

    // Simple deterministic 64‑bit FNV‑1a hash for reproducibility
    inline std::uint64_t fnv1a_hash(const std::string& data) noexcept {
        const std::uint64_t fnv_offset_basis = 0xcbf29ce484222325ULL;
        const std::uint64_t fnv_prime = 0x100000001b3ULL;
        std::uint64_t hash = fnv_offset_basis;
        for (unsigned char c : data) {
            hash ^= static_cast<std::uint64_t>(c);
            hash *= fnv_prime;
        }
        return hash;
    }

    class HyperLogLog {
    public:
        // precision p: number of bits used for the register index (4 ≤ p ≤ 16)
        explicit HyperLogLog(std::uint8_t precision = 12);

        // Add an element (by its string representation) to the sketch
        void add(const std::string& element);

        // Return the cardinality estimate
        double estimate() const;

        // Merge another sketch with identical precision into this one
        void merge(const HyperLogLog& other);

        // Number of registers (m = 2^p)
        std::size_t register_count() const noexcept { return registers.size(); };

    private:
        std::uint8_t p;                 // precision
        std::uint32_t m;                // number of registers = 2^p
        std::vector<std::uint8_t> registers; // registers storing max rank per bucket

        static double get_alpha(std::uint32_t m) noexcept;
        static std::uint8_t rho(std::uint64_t w, std::uint8_t max_bits) noexcept;
    };

} // namespace hll
