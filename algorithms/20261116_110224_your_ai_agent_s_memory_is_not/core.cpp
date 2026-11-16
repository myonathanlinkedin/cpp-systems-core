#include "core.hpp"

namespace lsm {

LRUCache::LRUCache(std::size_t capacity) : capacity_(capacity) {}

int LRUCache::get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return -1; // Key not present.
    }
    // Move the accessed node to the front (most recently used).
    usage_order_.splice(usage_order_.begin(), usage_order_, it->second);
    return it->second->value;
}

void LRUCache::put(int key, int value) {
    if (capacity_ == 0) {
        return; // Cache disabled.
    }

    auto it = map_.find(key);
    if (it != map_.end()) {
        // Key exists: update value and move to front.
        it->second->value = value;
        usage_order_.splice(usage_order_.begin(), usage_order_, it->second);
        return;
    }

    // Insert new node at front.
    usage_order_.push_front(Node{key, value});
    map_[key] = usage_order_.begin();

    // Evict least-recently used if over capacity.
    if (usage_order_.size() > capacity_) {
        Node& lru = usage_order_.back();
        map_.erase(lru.key);
        usage_order_.pop_back();
    }
}

} // namespace lsm
