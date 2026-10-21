#include <iostream>
#include <string>
#include <cassert>

#include "types.hpp"
#include "core.hpp"

int main() {
    using namespace lsm;
    HashTable<int, std::string> ht;

    // Test insert and find
    assert(ht.insert(1, "one"));
    assert(ht.insert(2, "two"));
    assert(ht.insert(3, "three"));
    assert(ht.size() == 3);
    assert(ht.find(1).value() == "one");
    assert(ht.find(2).value() == "two");
    assert(ht.find(3).value() == "three");
    assert(!ht.find(4).has_value());

    // Test update existing key
    assert(ht.insert(2, "TWO"));
    assert(ht.find(2).value() == "TWO");

    // Test erase
    assert(ht.erase(2));
    assert(!ht.find(2).has_value());
    assert(ht.size() == 2);
    assert(!ht.erase(2)); // already erased

    // Test rehashing by inserting many elements
    for (int i = 4; i <= 100; ++i) {
        assert(ht.insert(i, std::to_string(i)));
    }
    assert(ht.size() == 99);
    for (int i = 4; i <= 100; ++i) {
        assert(ht.find(i).value() == std::to_string(i));
    }

    // Test deletion of non-existent key
    assert(!ht.erase(200));

    // Test that load factor triggers rehash
    size_t old_capacity = ht.capacity();
    for (int i = 101; i <= 200; ++i) {
        ht.insert(i, std::to_string(i));
    }
    assert(ht.capacity() > old_capacity); // rehash occurred

    std::cout << "All tests passed. Final size: " << ht.size()
              << ", capacity: " << ht.capacity() << std::endl;
    return 0;
}
