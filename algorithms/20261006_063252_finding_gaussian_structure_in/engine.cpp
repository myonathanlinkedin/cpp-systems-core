#include <vector>
#include <cassert>

#include "types.hpp"

namespace bosonic {

double computeMean(const StateVector& state) {
    assert(!state.empty() && "State vector must not be empty");
    const auto prob = probabilityDistribution(state);
    double mean = 0.0;
    for (std::size_t n = 0; n < prob.size(); ++n) {
        mean += static_cast<double>(n) * prob[n];
    }
    return mean;
}

double computeVariance(const StateVector& state, double mean) {
    assert(!state.empty() && "State vector must not be empty");
    const auto prob = probabilityDistribution(state);
    double var = 0.0;
    for (std::size_t n = 0; n < prob.size(); ++n) {
        double diff = static_cast<double>(n) - mean;
        var += diff * diff * prob[n];
    }
    return var;
}

double computeFourthCentralMoment(const StateVector& state, double mean) {
    assert(!state.empty() && "State vector must not be empty");
    const auto prob = probabilityDistribution(state);
    double mu4 = 0.0;
    for (std::size_t n = 0; n < prob.size(); ++n) {
        double diff = static_cast<double>(n) - mean;
        mu4 += diff * diff * diff * diff * prob[n];
    }
    return mu4;
}

GaussianParameters fitGaussian(const StateVector& state) {
    double mean = computeMean(state);
    double var = computeVariance(state, mean);
    return GaussianParameters{mean, var};
}

bool isGaussian(const StateVector& state, double tolerance) {
    if (state.empty()) {
        return false;
    }
    double mean = computeMean(state);
    double var = computeVariance(state, mean);
    if (var <= 0.0) {
        return false;
    }
    double mu4 = computeFourthCentralMoment(state, mean);
    double kurtosis = mu4 / (var * var);
    // Gaussian distribution has kurtosis = 3
    return std::abs(kurtosis - 3.0) <= tolerance;
}

StateVector generateGaussianState(std::size_t maxPhotonNumber, double mean, double variance) {
    assert(maxPhotonNumber > 0 && "maxPhotonNumber must be positive");
    assert(variance > 0.0 && "variance must be positive");
    StateVector state;
    state.reserve(maxPhotonNumber + 1);
    const double denom = std::sqrt(2.0 * M_PI * variance);
    double sumProb = 0.0;
    std::vector<double> probs(maxPhotonNumber + 1, 0.0);
    for (std::size_t n = 0; n <= maxPhotonNumber; ++n) {
        double x = static_cast<double>(n);
        double exponent = - (x - mean) * (x - mean) / (2.0 * variance);
        double p = std::exp(exponent) / denom;
        probs[n] = p;
        sumProb += p;
    }
    // Normalize and convert to amplitudes
    for (std::size_t n = 0; n <= maxPhotonNumber; ++n) {
        double normalizedProb = probs[n] / sumProb;
        state.emplace_back(std::sqrt(normalizedProb), 0.0);
    }
    // Ensure the state is normalized (should be by construction)
    double norm = 0.0;
    for (const auto& amp : state) {
        norm += std::norm(amp);
    }
    double scale = 1.0 / std::sqrt(norm);
    for (auto& amp : state) {
        amp *= scale;
    }
    return state;
}

} // namespace bosonic
