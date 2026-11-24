#include <iostream>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

int main() {
    // Test 1: Basic increment/decrement
    PNCounter c1;
    c1.increment("nodeA");
    c1.increment("nodeA");
    c1.decrement("nodeA");
    assert(c1.value() == 1);

    // Test 2: Multiple nodes
    PNCounter c2;
    c2.increment("nodeB");
    c2.decrement("nodeA");
    assert(c2.value() == 0);

    // Test 3: Merge counters
    PNCounter merged = c1;
    merged.merge(c2);
    assert(merged.value() == 1); // 2 inc -1 dec +1 inc -1 dec = 1

    // Test 4: Idempotence
    PNCounter c3 = merged;
    c3.merge(merged);
    assert(c3.value() == merged.value());

    // Test 5: Commutativity
    PNCounter a, b;
    a.increment("nodeX");
    b.decrement("nodeY");
    PNCounter a_copy = a, b_copy = b;
    a_copy.merge(b_copy);
    b_copy.merge(a_copy);
    assert(a_copy.value() == b_copy.value());

    // Test 6: Associativity
    PNCounter x, y, z;
    x.increment("node1");
    y.increment("node2");
    z.decrement("node1");
    PNCounter xy = x;
    xy.merge(y);
    xy.merge(z);
    PNCounter yz = y;
    yz.merge(z);
    PNCounter x_yz = x;
    x_yz.merge(yz);
    assert(xy.value() == x_yz.value());

    // Test 7: Negative values
    PNCounter neg;
    neg.decrement("nodeNeg");
    neg.decrement("nodeNeg");
    assert(neg.value() == static_cast<uint64_t>(-2));

    // Test 8: Large counts
    PNCounter large;
    for (int i = 0; i < 1000; ++i) large.increment("nodeLarge");
    for (int i = 0; i < 500; ++i) large.decrement("nodeLarge");
    assert(large.value() == 500);

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
