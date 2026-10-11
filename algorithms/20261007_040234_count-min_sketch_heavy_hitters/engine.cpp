#include <vector>
#include <string>
#include <cmath>

#include "types.hpp"

CountMinSketch::CountMinSketch(double epsilon, double delta)
    : totalCount_(0)
{
    if (epsilon <= 0.0 || delta <= 0.0) {
        width_ = 1;
        depth_ = 1;
    } else {
        width_ = static_cast<size_t>(std::ceil(2.71828 / epsilon));
        depth_ = static_cast<size_t>(std::ceil(std::log(1.0 / delta)));
    }
    table_.resize(depth_, std::vector<uint64_t>(width_, 0));
    seeds_.reserve(depth_);
    for (size_t i = 0; i < depth_; ++i) {
        seeds_.push_back(std::hash<size_t>{}(i + 0x9e3779b9));
    }
}

size_t CountMinSketch::hash_with_seed(const std::string& key, size_t seed) const {
    size_t h = std::hash<std::string>{}(key);
    h ^= seed + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h % width_;
}

void CountMinSketch::add(const std::string& key, uint64_t count) {
    for (size_t i = 0; i < depth_; ++i) {
        size_t idx = hash_with_seed(key, seeds_[i]);
        table_[i][idx] += count;
    }
    totalCount_ += count;
    keys_.insert(key);
}

uint64_t CountMinSketch::estimate(const std::string& key) const {
    uint64_t minVal = UINT64_MAX;
    for (size_t i = 0; i < depth_; ++i) {
        size_t idx = hash_with_seed(key, seeds_[i]);
        uint64_t val = table_[i][idx];
        if (val < minVal) minVal = val;
    }
    return minVal;
}

std::vector<std::string> CountMinSketch::heavyHitters(double thresholdFraction) const {
    std::vector<std::string> result;
    if (keys_.empty() || totalCount_ == 0) return result;
    uint64_t threshold = static_cast<uint64_t>(totalCount_ * thresholdFraction);
    for (const auto& key : keys_) {
        if (estimate(key) >= threshold) {
            result.push_back(key);
        }
    }
    return result;
}
