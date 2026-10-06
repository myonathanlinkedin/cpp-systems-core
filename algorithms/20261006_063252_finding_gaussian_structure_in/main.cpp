#include <iostream>
#include <cassert>
#include "types.hpp"

int main() {
    using namespace bosonic;

    // Test 1: Gaussian state detection (should be true)
    {
        double mean = 10.0;
        double variance = 4.0;
        std::size_t maxN = 50;
        StateVector gaussState = generateGaussianState(maxN, mean, variance);
        assert(isGaussian(gaussState, 1e-5) && "Gaussian state not recognized as Gaussian");
        GaussianParameters params = fitGaussian(gaussState);
        assert(std::abs(params.mean - mean) < 0.2 && "Mean estimation error too large");
        assert(std::abs(params.variance - variance) < 0.5 && "Variance estimation error too large");
    }

    // Test 2: Uniform (non‑Gaussian) state detection (should be false)
    {
        std::size_t dim = 20;
        StateVector uniformState;
        uniformState.reserve(dim);
        double amp = 1.0 / std::sqrt(static_cast<double>(dim));
        for (std::size_t i = 0; i < dim; ++i) {
            uniformState.emplace_back(amp, 0.0);
        }
        assert(!isGaussian(uniformState, 1e-5) && "Uniform state incorrectly classified as Gaussian");
    }

    // Test 3: Edge case – empty state
    {
        StateVector emptyState;
        assert(!isGaussian(emptyState) && "Empty state should not be considered Gaussian");
    }

    // Test 4: Single‑photon Fock state (variance = 0, not Gaussian)
    {
        StateVector fock(5, Complex{0.0, 0.0});
        fock[2] = Complex{1.0, 0.0}; // photon number 2
        assert(!isGaussian(fock) && "Fock state incorrectly classified as Gaussian");
    }

    std::cout << "All unit tests passed.\n";
    return 0;
}
