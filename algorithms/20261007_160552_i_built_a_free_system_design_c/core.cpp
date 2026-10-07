#include <string>

#include "types.hpp"

namespace lsm {
    template<typename Key, typename Value>
    LRUCache<Key, Value>::LRUCache(size_t capacity) : capacity_(capacity) {}

    template<typename Key, typename Value>
    bool LRUCache<Key, Value>::get(const Key& key, Value& value) {
        auto it = map_.find(key);
        if (it == map_.end()) return false;
        move_to_front(it->second);
        value = it->second->second;
        return true;
    }

    template<typename Key, typename Value>
    void LRUCache<Key, Value>::put(const Key& key, const Value& value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            move_to_front(it->second);
            return;
        }
        if (items_.size() == capacity_) {
            auto last = items_.back();
            map_.erase(last.first);
            items_.pop_back();
        }
        items_.emplace_front(key, value);
        map_[key] = items_.begin();
    }

    template<typename Key, typename Value>
    size_t LRUCache<Key, Value>::size() const {
        return items_.size();
    }

    template<typename Key, typename Value>
    void LRUCache<Key, Value>::move_to_front(typename std::list<std::pair<Key, Value>>::iterator it) {
        items_.splice(items_.begin(), items_, it);
    }

    // Explicit template instantiation for common types
    template class LRUCache<int, int>;
    template class LRUCache<std::string, std::string>;
}
