#include <map>
#include <set>

#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>
#include <algorithm>

class PNCounter {
public:
    PNCounter() = default;
    PNCounter(const PNCounter&) = default;
    PNCounter& operator=(const PNCounter&) = default;
    PNCounter(PNCounter&&) noexcept = default;
    PNCounter& operator=(PNCounter&&) noexcept = default;
    ~PNCounter() = default;

    void increment(const std::string& node);
    void decrement(const std::string& node);
    uint64_t value() const;
    void merge(const PNCounter& other);

private:
    std::unordered_map<std::string, uint64_t> p_; // increments
    std::unordered_map<std::string, uint64_t> n_; // decrements
};
