#include <unordered_map>
#include <map>
#include <set>

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <functional>

class CountMinSketch {
public:
    // epsilon: error factor (relative), delta: probability of error
    explicit CountMinSketch(double epsilon, double delta);
    void add(const std::string& key, uint64_t count = 1);
    uint64_t estimate(const std::string& key) const;
    std::vector<std::string> heavyHitters(double thresholdFraction) const;

private:
    size_t width_;
    size_t depth_;
    std::vector<std::vector<uint64_t>> table_;
    std::vector<size_t> seeds_;
    uint64_t totalCount_;
    std::unordered_set<std::string> keys_;

    size_t hash_with_seed(const std::string& key, size_t seed) const;
};
