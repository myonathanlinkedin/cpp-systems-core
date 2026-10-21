#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>

#include "types.hpp"

namespace hll {

    HyperLogLog::HyperLogLog(std::uint8_t precision)
        : p(precision),
          m(1U << precision),
          registers(m, 0) {
        assert(p >= 4 && p <= 16 && "Precision must be in [4,16]");
    }

    double HyperLogLog::get_alpha(std::uint32_t m) noexcept {
        // Empirically derived constants
        if (m == 16)  return 0.673;
        if (m == 32)  return 0.697;
        if (m == 64)  return 0.709;
        return 0.7213 / (1.0 + 1.079 / static_cast<double>(m));
    }

    // Count leading zeros in the lower (64-p) bits, plus one.
    std::uint8_t HyperLogLog::rho(std::uint64_t w, std::uint8_t max_bits) noexcept {
        // w contains the bits after the index; we need position of first 1.
        // If w == 0, rank = max_bits + 1.
        if (w == 0) {
            return max_bits + 1;
        }
#if defined(__GNUC__) || defined(__clang__)
        int leading = __builtin_clzll(w);
        // __builtin_clzll counts leading zeros in 64‑bit value.
        // We only care about the lower (max_bits) bits, so shift left.
        int shift = 64 - max_bits;
        return static_cast<std::uint8_t>(leading - shift + 1);
#else
        // Portable fallback
        std::uint8_t rank = 1;
        while ((w & (static_cast<std::uint64_t>(1) << (max_bits - rank))) == 0 && rank <= max_bits) {
            ++rank;
        }
        return rank;
#endif
    }

    void HyperLogLog::add(const std::string& element) {
        std::uint64_t hash = fnv1a_hash(element);
        // Extract the first p bits as register index
        std::uint32_t idx = static_cast<std::uint32_t>(hash >> (64 - p));
        // Remaining bits
        std::uint64_t w = hash << p; // shift left to discard index bits
        std::uint8_t rank = rho(w, 64 - p);
        if (rank > registers[idx]) {
            registers[idx] = rank;
        }
    }

    void HyperLogLog::merge(const HyperLogLog& other) {
        assert(p == other.p && "Cannot merge HLL sketches with different precision");
        for (std::size_t i = 0; i < registers.size(); ++i) {
            registers[i] = std::max(registers[i], other.registers[i]);
        }
    }

    double HyperLogLog::estimate() const {
        double alpha = get_alpha(m);
        double Z = 0.0;
        for (std::uint8_t reg : registers) {
            Z += std::pow(2.0, -static_cast<double>(reg));
        }
        Z = 1.0 / Z;
        double raw_est = alpha * static_cast<double>(m) * static_cast<double>(m) * Z;

        // Small range correction (Linear Counting)
        std::size_t V = std::count(registers.begin(), registers.end(), static_cast<std::uint8_t>(0));
        if (raw_est <= 5.0 / 2.0 * static_cast<double>(m) && V != 0) {
            raw_est = static_cast<double>(m) * std::log(static_cast<double>(m) / static_cast<double>(V));
        }

        // Large range correction
        const double two64 = std::pow(2.0, 64);
        if (raw_est > (1.0 / 30.0) * two64) {
            raw_est = -two64 * std::log(1.0 - raw_est / two64);
        }

        // For empty sketch, return 0
        if (V == m) {
            return 0.0;
        }

        return raw_est;
    }

} // namespace hll
