#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>

#include "core.hpp"

namespace hll {

HyperLogLog::HyperLogLog(uint8_t precision)
    : p_(precision),
      m_(static_cast<std::size_t>(1) << precision),
      registers_(m_, 0) {
    // Valid precision range check (4 ≤ p ≤ 16) to keep memory reasonable.
    assert(p_ >= 4 && p_ <= 16);
}

void HyperLogLog::addHash(uint64_t hash) {
    // Split the hash: the first p bits are the register index.
    uint64_t idx = hash >> (64 - p_);
    uint64_t w = (hash << p_) | (1ULL << (p_ - 1)); // ensure w != 0
    uint8_t rank = rho(w, 64 - p_);
    if (rank > registers_[static_cast<std::size_t>(idx)]) {
        registers_[static_cast<std::size_t>(idx)] = rank;
    }
}

template <typename T>
void HyperLogLog::add(const T& element) {
    // Use a deterministic 64‑bit hash (splitmix64 on std::hash result).
    static_assert(sizeof(std::size_t) <= 8, "size_t must be ≤ 64 bits");
    std::size_t h = std::hash<T>{}(element);
    // Expand to 64 bits via splitmix64.
    uint64_t x = static_cast<uint64_t>(h);
    // SplitMix64 scramble
    x += 0x9e3779b97f4a7c15ULL;
    uint64_t z = x;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    addHash(z);
}

// Explicit template instantiation for common types used in tests.
template void HyperLogLog::add<int>(const int&);
template void HyperLogLog::add<std::string>(const std::string&);

double HyperLogLog::estimate() const {
    double alpha = getAlpha(p_);
    double Z = 0.0;
    for (uint8_t reg : registers_) {
        Z += std::pow(2.0, -static_cast<double>(reg));
    }
    double raw_est = alpha * static_cast<double>(m_) * static_cast<double>(m_) / Z;

    // Small range correction (Linear Counting)
    std::size_t V = std::count(registers_.begin(), registers_.end(), 0);
    if (raw_est <= 5.0 * static_cast<double>(m_) / 2.0 && V != 0) {
        double corrected = static_cast<double>(m_) * std::log(static_cast<double>(m_) / V);
        return corrected;
    }

    // Large range correction
    constexpr double TWO64 = static_cast<double>(18446744073709551616.0); // 2^64
    if (raw_est > (1.0 / 30.0) * TWO64) {
        double corrected = -TWO64 * std::log(1.0 - raw_est / TWO64);
        return corrected;
    }

    return raw_est;
}

void HyperLogLog::merge(const HyperLogLog& other) {
    assert(p_ == other.p_ && "Precision must match for merge");
    for (std::size_t i = 0; i < m_; ++i) {
        registers_[i] = std::max(registers_[i], other.registers_[i]);
    }
}

// ----- Private helpers -----
constexpr double HyperLogLog::getAlpha(uint8_t p) {
    std::size_t m = static_cast<std::size_t>(1) << p;
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1.0 + 1.079 / static_cast<double>(m));
}

// Count leading zeros in w (up to maxBits) plus one.
uint8_t HyperLogLog::rho(uint64_t w, uint8_t maxBits) {
    uint8_t leading = 0;
    for (uint8_t i = 0; i < maxBits; ++i) {
        if ((w & (1ULL << (63 - i))) == 0) {
            ++leading;
        } else {
            break;
        }
    }
    return leading + 1;
}

} // namespace hll
