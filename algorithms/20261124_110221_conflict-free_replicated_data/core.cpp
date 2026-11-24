#include <string>

#include "core.hpp"

void PNCounter::increment(const std::string& node) {
    ++p_[node];
}

void PNCounter::decrement(const std::string& node) {
    ++n_[node];
}

uint64_t PNCounter::value() const {
    uint64_t sumP = 0;
    for (const auto& kv : p_) {
        sumP += kv.second;
    }
    uint64_t sumN = 0;
    for (const auto& kv : n_) {
        sumN += kv.second;
    }
    return sumP - sumN;
}

void PNCounter::merge(const PNCounter& other) {
    for (const auto& kv : other.p_) {
        auto it = p_.find(kv.first);
        if (it == p_.end() || kv.second > it->second) {
            p_[kv.first] = kv.second;
        }
    }
    for (const auto& kv : other.n_) {
        auto it = n_.find(kv.first);
        if (it == n_.end() || kv.second > it->second) {
            n_[kv.first] = kv.second;
        }
    }
}
