#include <map>
#include <set>

#pragma once
#include <unordered_map>
#include <list>
#include <utility>
#include <cstddef>
#include <string>

namespace lsm {
    template<typename Key, typename Value>
    class LRUCache {
    public:
        explicit LRUCache(size_t capacity);
        bool get(const Key& key, Value& value);
        void put(const Key& key, const Value& value);
        size_t size() const;
    private:
        size_t capacity_;
        std::list<std::pair<Key, Value>> items_;
        std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> map_;
        void move_to_front(typename std::list<std::pair<Key, Value>>::iterator it);
    };
}
