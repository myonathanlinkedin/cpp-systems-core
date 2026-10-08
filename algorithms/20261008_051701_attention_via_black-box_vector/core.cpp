#include <algorithm>

#include "core.hpp"
#include <numeric>
#include <cmath>
#include <cassert>

Vector dot(const Vector& a, const Vector& b) {
    assert(a.size() == b.size());
    return {std::inner_product(a.begin(), a.end(), b.begin(), 0.0)};
}

Vector softmax(const Vector& scores) {
    double max_val = *std::max_element(scores.begin(), scores.end());
    Vector exp_scores;
    exp_scores.reserve(scores.size());
    double sum = 0.0;
    for (double s : scores) {
        double e = std::exp(s - max_val);
        exp_scores.push_back(e);
        sum += e;
    }
    for (double& e : exp_scores) e /= sum;
    return exp_scores;
}

Vector attention(const Vector& query,
                 const Matrix& keys,
                 const Matrix& values) {
    assert(keys.size() == values.size());
    size_t n = keys.size();
    if (n == 0) return {};

    // Compute similarity scores
    Vector scores;
    scores.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        assert(query.size() == keys[i].size());
        scores.push_back(dot(query, keys[i])[0]);
    }

    // Compute attention weights
    Vector weights = softmax(scores);

    // Weighted sum of values
    size_t dim = values[0].size();
    for (const auto& val : values) assert(val.size() == dim);
    Vector result(dim, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t d = 0; d < dim; ++d) {
            result[d] += weights[i] * values[i][d];
        }
    }
    return result;
}
