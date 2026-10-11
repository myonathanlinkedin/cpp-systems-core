#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

int main() {
    using namespace lsm;
    LSMTree db(3); // small threshold for testing

    // Test basic put/get
    db.put("a", "1");
    db.put("b", "2");
    assert(db.get("a").value() == "1");
    assert(db.get("b").value() == "2");
    assert(!db.get("c").has_value());

    // Test overwrite
    db.put("a", "10");
    assert(db.get("a").value() == "10");

    // Trigger flush by exceeding threshold
    db.put("c", "3");
    db.put("d", "4"); // threshold reached, flush occurs
    assert(db.get("c").value() == "3");
    assert(db.get("d").value() == "4");

    // Test after flush
    db.put("e", "5");
    assert(db.get("e").value() == "5");

    // Test compaction
    db.compact();
    assert(db.get("a").value() == "10");
    assert(db.get("b").value() == "2");
    assert(db.get("c").value() == "3");
    assert(db.get("d").value() == "4");
    assert(db.get("e").value() == "5");

    // Test large number of operations
    LSMTree big_db(1000);
    for (int i = 0; i < 5000; ++i) {
        big_db.put("key" + std::to_string(i), "val" + std::to_string(i));
    }
    for (int i = 0; i < 5000; ++i) {
        auto val = big_db.get("key" + std::to_string(i));
        assert(val.has_value() && val.value() == "val" + std::to_string(i));
    }
    std::cout << "All tests passed.\n";
    return 0;
}
