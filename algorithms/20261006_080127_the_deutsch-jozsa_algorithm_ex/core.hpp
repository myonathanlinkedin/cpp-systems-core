#pragma once
#include "types.hpp"
#include <vector>
#include <functional>
#include <cassert>

namespace dj {

/**
 * @brief Quantum state for n+1 qubits (n input qubits + 1 ancilla).
 */
class QuantumState {
public:
    explicit QuantumState(std::size_t n_input_qubits);
    void applyHadamardAll();                     // Hadamard on every qubit
    void applyHadamard(std::size_t qubit);       // Hadamard on a single qubit
    void applyOracle(const std::function<int(BitString)>& f); // phase oracle
    double probabilityAllZeroInput() const;     // prob that first n bits are 0

private:
    std::size_t n_input_;        // number of input qubits (excluding ancilla)
    std::size_t total_qubits_;   // n_input_ + 1
    std::vector<Complex> amp_;   // state vector, size = 2^{total_qubits_};
    static constexpr double SQRT2_INV = 0.7071067811865475; // 1/sqrt(2)

    inline std::size_t index(std::size_t basis) const noexcept { return basis; }
    static inline bool getBit(BitString x, std::size_t pos) noexcept { return (x >> pos) & 1ULL; }
    static inline BitString setBit(BitString x, std::size_t pos, bool val) noexcept {
        return val ? (x | (1ULL << pos)) : (x & ~(1ULL << pos));
    }
};

/**
 * @brief Executes the Deutsch-Jozsa algorithm on a Boolean function f.
 *
 * @param f Callable returning 0 or 1 for a given input BitString (n bits).
 * @param n Number of input bits (n >= 1).
 * @return true if f is constant, false if f is balanced.
 */
bool deutschJozsa(const std::function<int(BitString)>& f, std::size_t n);

} // namespace dj
