#include <cassert>

#include "core.hpp"
#include <cmath>

namespace dj {

QuantumState::QuantumState(std::size_t n_input_qubits)
    : n_input_(n_input_qubits),
      total_qubits_(n_input_qubits + 1),
      amp_(static_cast<std::size_t>(1ULL << total_qubits_), Complex{0.0, 0.0}) {
    // Initialize |0...0⟩|1⟩ (ancilla in |1⟩)
    std::size_t init_index = 1ULL << n_input_qubits; // ancilla bit = 1
    amp_[init_index] = Complex{1.0, 0.0};
}

void QuantumState::applyHadamard(std::size_t qubit) {
    std::size_t stride = 1ULL << qubit;
    std::size_t block = stride << 1;
    for (std::size_t i = 0; i < amp_.size(); i += block) {
        for (std::size_t j = 0; j < stride; ++j) {
            Complex a = amp_[i + j];
            Complex b = amp_[i + j + stride];
            amp_[i + j] = (a + b) * SQRT2_INV;
            amp_[i + j + stride] = (a - b) * SQRT2_INV;
        }
    }
}

void QuantumState::applyHadamardAll() {
    for (std::size_t q = 0; q < total_qubits_; ++q) {
        applyHadamard(q);
    }
}

void QuantumState::applyOracle(const std::function<int(BitString)>& f) {
    // Phase flip: multiply amplitude by (-1)^{f(x)} where x are the first n_input_ bits.
    for (std::size_t idx = 0; idx < amp_.size(); ++idx) {
        BitString x = idx & ((1ULL << n_input_) - 1ULL); // extract input bits
        if (f(x) & 1) {
            amp_[idx] *= Complex{-1.0, 0.0};
        }
    }
}

double QuantumState::probabilityAllZeroInput() const {
    double prob = 0.0;
    for (std::size_t idx = 0; idx < amp_.size(); ++idx) {
        BitString x = idx & ((1ULL << n_input_) - 1ULL);
        if (x == 0) {
            prob += std::norm(amp_[idx]);
        }
    }
    return prob;
}

bool deutschJozsa(const std::function<int(BitString)>& f, std::size_t n) {
    assert(n >= 1);
    QuantumState qs(n);
    qs.applyHadamardAll();          // H^{⊗(n+1)}
    qs.applyOracle(f);              // U_f
    // Apply Hadamard only on the first n qubits
    for (std::size_t q = 0; q < n; ++q) {
        qs.applyHadamard(q);
    }
    double prob_zero = qs.probabilityAllZeroInput();
    // In ideal case, constant => prob ≈ 1, balanced => prob ≈ 0
    constexpr double EPS = 1e-9;
    return prob_zero > 0.5 - EPS;
}

} // namespace dj
