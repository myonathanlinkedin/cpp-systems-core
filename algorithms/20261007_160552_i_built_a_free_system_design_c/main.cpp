#include <iostream>
#include <cassert>
#include <string>
#include "types.hpp"
#include "core.hpp"

int main() {
    using namespace lsm;
    LRUCache<int, int> cache(2);
    int val;
    assert(!cache.get(1, val));
    cache.put(1, 10);
    assert(cache.get(1, val) && val == 10);
    cache.put(2, 20);
    assert(cache.get(2, val) && val == 20);
    cache.put(3, 30); // evicts key 1
    assert(!cache.get(1, val));
    assert(cache.get(3, val) && val == 30);
    assert(cache.size() == 2);
    std::cout << "All tests passed.\n";
    return 0;
}
