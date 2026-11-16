#include <map>
#include <set>

#pragma once

#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>

namespace lsm {

class LRUCache {
public:
    // Constructs an LRU cache with the given capacity.
    // Capacity of 0 creates a cache that never stores items.
    explicit LRUCache(std::size_t capacity);

    // Retrieves the value associated with `key`.
    // Returns -1 if the key is not present.
    int get(int key);

    // Inserts or updates the value for `key`.
    // If the cache exceeds its capacity, the least-recently used entry is evicted.
    void put(int key, int value);

private:
    // Internal node stored in the usage order list.
    struct Node {
        int key;
        int value;
    };

    // Maximum number of entries the cache may hold.
    std::size_t capacity_;

    // Doubly‑linked list of nodes; front = most recently used, back = least recently used.
    std::list<Node> usage_order_;

    // Maps a key to its corresponding node iterator in `usage_order_`.
    std::unordered_map<int, std::list<Node>::iterator> map_;
};

} // namespace lsm
