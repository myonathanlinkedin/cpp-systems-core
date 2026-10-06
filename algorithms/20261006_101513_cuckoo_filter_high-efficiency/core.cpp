#include "core.hpp"
#include <algorithm>

CuckooFilter::CuckooFilter(const CuckooFilterConfig& cfg)
    : cfg_(cfg),
      buckets_(cfg.bucket_count),
      item_count_(0),
      rng_(std::random_device{}())
{
    for (auto& bucket : buckets_) {
        bucket.fill(0);
    }
}

CuckooFilter::Fingerprint CuckooFilter::fingerprint(uint64_t item) const {
    // Simple 8‑bit fingerprint, never zero (zero denotes empty slot)
    uint64_t h = std::hash<uint64_t>{}(item);
    Fingerprint fp = static_cast<Fingerprint>(h & 0xFF);
    return fp ? fp : 1;
}

std::size_t CuckooFilter::index_hash(uint64_t item) const {
    uint64_t h = std::hash<uint64_t>{}(item);
    return static_cast<std::size_t>(h) & (cfg_.bucket_count - 1);
}

std::size_t CuckooFilter::alt_index(std::size_t index, Fingerprint fp) const {
    uint64_t h = std::hash<Fingerprint>{}(fp);
    return (index ^ static_cast<std::size_t>(h)) & (cfg_.bucket_count - 1);
}

bool CuckooFilter::insert_into_bucket(std::size_t index, Fingerprint fp) {
    Bucket& bucket = buckets_[index];
    for (auto& entry : bucket) {
        if (entry == 0) {
            entry = fp;
            ++item_count_;
            return true;
        }
    }
    return false;
}

bool CuckooFilter::bucket_contains(std::size_t index, Fingerprint fp) const {
    const Bucket& bucket = buckets_[index];
    return std::any_of(bucket.begin(), bucket.end(),
                       [fp](Fingerprint e) { return e == fp; });
}

bool CuckooFilter::delete_from_bucket(std::size_t index, Fingerprint fp) {
    Bucket& bucket = buckets_[index];
    for (auto& entry : bucket) {
        if (entry == fp) {
            entry = 0;
            --item_count_;
            return true;
        }
    }
    return false;
}

bool CuckooFilter::insert(uint64_t item) {
    Fingerprint fp = fingerprint(item);
    std::size_t i1 = index_hash(item);
    std::size_t i2 = alt_index(i1, fp);

    if (insert_into_bucket(i1, fp) || insert_into_bucket(i2, fp))
        return true;

    std::size_t cur_index = (rng_() & 1) ? i1 : i2;
    Fingerprint cur_fp = fp;

    for (std::size_t kick = 0; kick < cfg_.max_kicks; ++kick) {
        Bucket& bucket = buckets_[cur_index];
        std::uniform_int_distribution<std::size_t> dist(0, cfg_.bucket_size - 1);
        std::size_t evict_pos = dist(rng_);
        std::swap(bucket[evict_pos], cur_fp);
        cur_index = alt_index(cur_index, cur_fp);
        if (insert_into_bucket(cur_index, cur_fp))
            return true;
    }
    return false; // filter is considered full
}

bool CuckooFilter::contains(uint64_t item) const {
    Fingerprint fp = fingerprint(item);
    std::size_t i1 = index_hash(item);
    std::size_t i2 = alt_index(i1, fp);
    return bucket_contains(i1, fp) || bucket_contains(i2, fp);
}

bool CuckooFilter::erase(uint64_t item) {
    Fingerprint fp = fingerprint(item);
    std::size_t i1 = index_hash(item);
    std::size_t i2 = alt_index(i1, fp);
    if (delete_from_bucket(i1, fp) || delete_from_bucket(i2, fp))
        return true;
    return false;
}
