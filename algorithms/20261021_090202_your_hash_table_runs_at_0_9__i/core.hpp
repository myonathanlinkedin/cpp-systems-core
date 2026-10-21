#include <vector>

#pragma once
#include "types.hpp"

namespace lsm {

template <typename K, typename V>
class HashTable {
public:
    HashTable(size_t initial_capacity = 8);
    bool insert(const K& key, const V& value);
    std::optional<V> find(const K& key) const;
    bool erase(const K& key);
    size_t size() const { return count_; };
    size_t capacity() const { return table_.size(); }

private:
    std::vector<HashEntry<K, V>> table_;
    size_t count_ = 0;
    static constexpr double max_load_factor_ = 0.9;
    void rehash();
    size_t next_prime(size_t n) const;
};

} // namespace lsm
