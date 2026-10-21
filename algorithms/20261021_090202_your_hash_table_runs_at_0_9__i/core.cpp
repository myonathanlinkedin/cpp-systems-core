#include <vector>
#include <string>

#include "core.hpp"

namespace lsm {

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t initial_capacity) {
    size_t cap = next_prime(initial_capacity);
    table_.resize(cap);
}

template <typename K, typename V>
size_t HashTable<K, V>::next_prime(size_t n) const {
    auto is_prime = [](size_t x) {
        if (x < 2) return false;
        if (x % 2 == 0) return x == 2;
        for (size_t i = 3; i * i <= x; i += 2)
            if (x % i == 0) return false;
        return true;
    };
    while (!is_prime(n)) ++n;
    return n;
}

template <typename K, typename V>
void HashTable<K, V>::rehash() {
    size_t old_capacity = table_.size();
    std::vector<HashEntry<K, V>> old_table = std::move(table_);
    size_t new_capacity = next_prime(old_capacity * 2);
    table_.clear();
    table_.resize(new_capacity);
    count_ = 0;
    for (auto& entry : old_table) {
        if (entry.state == EntryState::Occupied) {
            insert(entry.key, entry.value);
        }
    }
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const K& key, const V& value) {
    if (static_cast<double>(count_ + 1) / table_.size() > max_load_factor_) {
        rehash();
    }
    size_t idx = std::hash<K>{}(key) % table_.size();
    size_t first_deleted = table_.size(); // sentinel
    for (size_t i = 0; i < table_.size(); ++i) {
        size_t probe = (idx + i) % table_.size();
        auto& entry = table_[probe];
        if (entry.state == EntryState::Empty) {
            if (first_deleted != table_.size()) {
                table_[first_deleted] = HashEntry<K, V>(key, value);
            } else {
                entry = HashEntry<K, V>(key, value);
            }
            ++count_;
            return true;
        } else if (entry.state == EntryState::Deleted) {
            if (first_deleted == table_.size()) first_deleted = probe;
        } else if (entry.state == EntryState::Occupied && entry.key == key) {
            entry.value = value; // update
            return true;
        }
    }
    // Should not reach here
    return false;
}

template <typename K, typename V>
std::optional<V> HashTable<K, V>::find(const K& key) const {
    size_t idx = std::hash<K>{}(key) % table_.size();
    for (size_t i = 0; i < table_.size(); ++i) {
        size_t probe = (idx + i) % table_.size();
        const auto& entry = table_[probe];
        if (entry.state == EntryState::Empty) {
            return std::nullopt;
        } else if (entry.state == EntryState::Occupied && entry.key == key) {
            return entry.value;
        }
    }
    return std::nullopt;
}

template <typename K, typename V>
bool HashTable<K, V>::erase(const K& key) {
    size_t idx = std::hash<K>{}(key) % table_.size();
    for (size_t i = 0; i < table_.size(); ++i) {
        size_t probe = (idx + i) % table_.size();
        auto& entry = table_[probe];
        if (entry.state == EntryState::Empty) {
            return false;
        } else if (entry.state == EntryState::Occupied && entry.key == key) {
            entry.state = EntryState::Deleted;
            --count_;
            return true;
        }
    }
    return false;
}

// Explicit template instantiation for common types
template class HashTable<int, std::string>;
template class HashTable<std::string, int>;

} // namespace lsm
