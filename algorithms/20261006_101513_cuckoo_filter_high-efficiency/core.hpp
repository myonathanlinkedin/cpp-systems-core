#pragma once
#include "types.hpp"
#include <vector>
#include <array>
#include <random>
#include <functional>
#include <cstdint>

class CuckooFilter {
public:
    explicit CuckooFilter(const CuckooFilterConfig& cfg = CuckooFilterConfig());

    bool insert(uint64_t item);
    bool contains(uint64_t item) const;
    bool erase(uint64_t item);

    std::size_t size() const noexcept { return item_count_; };
    std::size_t capacity() const noexcept { return cfg_.bucket_count * cfg_.bucket_size; }

private:
    using Fingerprint = uint8_t;
    using Bucket = std::array<Fingerprint, 4>;

    CuckooFilterConfig cfg_;
    std::vector<Bucket> buckets_;
    std::size_t item_count_;
    mutable std::mt19937 rng_;

    Fingerprint fingerprint(uint64_t item) const;
    std::size_t index_hash(uint64_t item) const;
    std::size_t alt_index(std::size_t index, Fingerprint fp) const;
    bool insert_into_bucket(std::size_t index, Fingerprint fp);
    bool delete_from_bucket(std::size_t index, Fingerprint fp);
    bool bucket_contains(std::size_t index, Fingerprint fp) const;
};
