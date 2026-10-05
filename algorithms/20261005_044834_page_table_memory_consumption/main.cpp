#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <limits>
#include <cmath>

/**
 * @brief Utility namespace for power-of-two calculations.
 */
namespace util {
    /**
     * @brief Compute 2 raised to the power of exp.
     * @param exp Exponent (must be non‑negative and fit in 64 bits).
     * @return 2^exp as std::uint64_t.
     * @throws std::overflow_error if the result would overflow std::uint64_t.
     */
    constexpr std::uint64_t pow2(std::uint8_t exp) {
        if (exp >= 64) {
            throw std::overflow_error("Exponent too large for 64‑bit result");
        }
        return static_cast<std::uint64_t>(1) << exp;
    }
}

/**
 * @brief Class responsible for estimating the memory consumption of hierarchical page tables.
 *
 * The model assumes:
 *   - A virtual address space of `virtual_address_bits` bits.
 *   - A page size of `page_size` bytes (must be a power of two).
 *   - Page‑table entries of `entry_size` bytes (typically 8 bytes on 64‑bit systems).
 *   - A fixed number of levels `levels` (e.g., 4 for x86‑64 4‑level paging).
 *
 * The virtual address is split into:
 *   - `offset_bits` = log2(page_size) low‑order bits (page offset).
 *   - The remaining bits are evenly divided among the levels.
 *
 * The worst‑case memory consumption (full mapping) is:
 *   Σ_{i=1}^{levels} (2^{bits_per_level}) * entry_size
 *
 * If the bits cannot be evenly divided, the higher levels receive one extra bit.
 */
class PageTableMemoryCalculator {
public:
    /**
     * @brief Construct a calculator with the given parameters.
     * @param virtual_address_bits Width of the virtual address space in bits (e.g., 48).
     * @param page_size Size of a page in bytes (must be power of two, >= 4096).
     * @param entry_size Size of a single page‑table entry in bytes (must be > 0).
     * @param levels Number of hierarchical levels (>= 1).
     * @throws std::invalid_argument on invalid parameter combinations.
     */
    PageTableMemoryCalculator(std::uint8_t virtual_address_bits,
                              std::uint64_t page_size,
                              std::uint8_t entry_size,
                              std::uint8_t levels)
        : vaddr_bits_(virtual_address_bits),
          page_size_(page_size),
          entry_size_(entry_size),
          levels_(levels)
    {
        validate_parameters();
        offset_bits_ = static_cast<std::uint8_t>(std::log2(page_size_));
        bits_for_index_ = vaddr_bits_ - offset_bits_;
        distribute_bits_per_level();
    }

    /**
     * @brief Compute the worst‑case memory consumption of the page tables.
     * @return Memory consumption in bytes.
     * @throws std::overflow_error if the calculation exceeds 64‑bit range.
     */
    std::uint64_t compute_memory_consumption() const {
        std::uint64_t total_bytes = 0;
        for (std::uint8_t level = 0; level < levels_; ++level) {
            std::uint8_t bits = bits_per_level_[level];
            std::uint64_t entries = util::pow2(bits);
            // Guard against multiplication overflow
            if (entries > std::numeric_limits<std::uint64_t>::max() / entry_size_) {
                throw std::overflow_error("Entry count multiplication overflow");
            }
            std::uint64_t level_bytes = entries * entry_size_;
            if (total_bytes > std::numeric_limits<std::uint64_t>::max() - level_bytes) {
                throw std::overflow_error("Total memory accumulation overflow");
            }
            total_bytes += level_bytes;
        }
        return total_bytes;
    }

    /**
     * @brief Retrieve the number of bits allocated to each level (read‑only).
     * @return Array of size `levels` containing bits per level.
     */
    const std::uint8_t* bits_per_level() const { return bits_per_level_; }

private:
    std::uint8_t vaddr_bits_;
    std::uint64_t page_size_;
    std::uint8_t entry_size_;
    std::uint8_t levels_;
    std::uint8_t offset_bits_;
    std::uint8_t bits_for_index_;
    std::uint8_t bits_per_level_[8] = {}; // support up to 8 levels

    void validate_parameters() const {
        if (vaddr_bits_ == 0 || vaddr_bits_ > 64) {
            throw std::invalid_argument("Virtual address bits must be in (0,64]");
        }
        if (page_size_ < 4096) {
            throw std::invalid_argument("Page size must be at least 4096 bytes");
        }
        if ((page_size_ & (page_size_ - 1)) != 0) {
            throw std::invalid_argument("Page size must be a power of two");
        }
        if (entry_size_ == 0) {
            throw std::invalid_argument("Entry size must be non‑zero");
        }
        if (levels_ == 0 || levels_ > 8) {
            throw std::invalid_argument("Levels must be in [1,8]");
        }
        if (std::log2(page_size_) != static_cast<int>(std::log2(page_size_))) {
            throw std::invalid_argument("Page size must be an exact power of two");
        }
        if (vaddr_bits_ <= std::log2(page_size_)) {
            throw std::invalid_argument("Virtual address bits must exceed offset bits");
        }
    }

    void distribute_bits_per_level() {
        // Evenly distribute bits_for_index_ among levels, giving extra bits to lower levels.
        std::uint8_t base = bits_for_index_ / levels_;
        std::uint8_t remainder = bits_for_index_ % levels_;
        for (std::uint8_t i = 0; i < levels_; ++i) {
            bits_per_level_[i] = base + (i < remainder ? 1 : 0);
        }
    }
};

/**
 * @brief Simple test harness exercising typical and edge cases.
 */
int main() {
    // Test 1: x86‑64 typical configuration (48‑bit VA, 4 KB pages, 8‑byte entries, 4 levels)
    {
        PageTableMemoryCalculator calc(48, 4096, 8, 4);
        std::uint64_t mem = calc.compute_memory_consumption();
        // Expected bits per level: (48‑12)=36 bits => 9 bits per level
        // Entries per level = 2^9 = 512, total entries = 4*512 = 2048
        // Memory = 2048 * 8 = 16384 bytes
        assert(mem == 16384);
    }

    // Test 2: 39‑bit VA (ARMv8 3‑level), 4 KB pages, 8‑byte entries, 3 levels
    {
        PageTableMemoryCalculator calc(39, 4096, 8, 3);
        std::uint64_t mem = calc.compute_memory_consumption();
        // bits_for_index = 39‑12 = 27, distribute 9 bits each level
        // entries per level = 512, total = 3*512 = 1536, memory = 12288
        assert(mem == 12288);
    }

    // Test 3: Uneven distribution (47‑bit VA, 4 KB pages, 8‑byte entries, 4 levels)
    {
        PageTableMemoryCalculator calc(47, 4096, 8, 4);
        const std::uint8_t* bits = calc.bits_per_level();
        // bits_for_index = 35 => base 8, remainder 3 => [9,9,9,8]
        assert(bits[0] == 9 && bits[1] == 9 && bits[2] == 9 && bits[3] == 8);
        std::uint64_t mem = calc.compute_memory_consumption();
        // entries: 2^9 + 2^9 + 2^9 + 2^8 = 512+512+512+256 = 1792
        // memory = 1792 * 8 = 14336
        assert(mem == 14336);
    }

    // Test 4: Invalid parameters trigger exceptions
    {
        bool caught = false;
        try {
            PageTableMemoryCalculator calc(32, 3000, 8, 2); // non‑power‑of‑two page size
        } catch (const std::invalid_argument&) {
            caught = true;
        }
        assert(caught);
    }

    // Test 5: Overflow detection (use large virtual address space)
    {
        bool caught = false;
        try {
            PageTableMemoryCalculator calc(64, 4096, 8, 4);
            calc.compute_memory_consumption(); // should overflow 64‑bit
        } catch (const std::overflow_error&) {
            caught = true;
        }
        assert(caught);
    }

    std::cout << "All tests passed.\n";
    return 0;
}