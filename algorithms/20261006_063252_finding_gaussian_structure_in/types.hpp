#pragma once

#include <vector>
#include <complex>
#include <cstddef>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cassert>

using Complex = std::complex<double>;
using StateVector = std::vector<Complex>;

struct GaussianParameters {
    double mean;
    double variance;
};

namespace bosonic {

// Compute the probability distribution from a state vector (|amplitude|^2)
inline std::vector<double> probabilityDistribution(const StateVector& state) {
    std::vector<double> prob;
    prob.reserve(state.size());
    for (const auto& amp : state) {
        prob.push_back(std::norm(amp));
    }
    double sum = std::accumulate(prob.begin(), prob.end(), 0.0);
    assert(sum > 0.0 && "State vector must have non‑zero norm");
    for (auto& p : prob) {
        p /= sum;
    }
    return prob;
}

// Compute the first moment (mean) of the photon-number distribution
double computeMean(const StateVector& state);

// Compute the second central moment (variance) of the photon-number distribution
double computeVariance(const StateVector& state, double mean);

// Compute the fourth central moment (mu4) of the photon-number distribution
double computeFourthCentralMoment(const StateVector& state, double mean);

// Fit a Gaussian to the given bosonic state (returns mean and variance)
GaussianParameters fitGaussian(const StateVector& state);

// Test whether the state is Gaussian within a given tolerance (default 1e-6)
bool isGaussian(const StateVector& state, double tolerance = 1e-6);

// Generate a discrete approximation of a Gaussian bosonic state
StateVector generateGaussianState(std::size_t maxPhotonNumber, double mean, double variance);

} // namespace bosonic
