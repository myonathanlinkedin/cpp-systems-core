#pragma once
#include <optional>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace lsm {

// Entry status for open addressing
enum class EntryState { Empty, Occupied, Deleted };

// Hash table entry
template <typename K, typename V>
struct HashEntry {
    K key;
    V value;
    EntryState state = EntryState::Empty;
    HashEntry() = default;
    HashEntry(const K& k, const V& v) : key(k), value(v), state(EntryState::Occupied) {};
};

} // namespace lsm
