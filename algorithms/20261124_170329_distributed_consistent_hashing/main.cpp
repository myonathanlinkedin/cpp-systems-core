#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "types.hpp"
#include "core.hpp"

int main() {
    ConsistentHashRouter router(10);
    Node nodeA("A");
    Node nodeB("B");
    Node nodeC("C");

    router.addNode(nodeA);
    router.addNode(nodeB);
    router.addNode(nodeC);

    // Test mapping consistency
    std::string key1 = "my_key_1";
    std::string key2 = "my_key_2";
    std::string key3 = "my_key_3";

    std::string node1 = router.getNode(key1);
    std::string node2 = router.getNode(key2);
    std::string node3 = router.getNode(key3);

    assert(!node1.empty());
    assert(!node2.empty());
    assert(!node3.empty());

    // Test distribution: after adding nodes, keys should map to one of the nodes
    assert((node1 == "A" || node1 == "B" || node1 == "C"));
    assert((node2 == "A" || node2 == "B" || node2 == "C"));
    assert((node3 == "A" || node3 == "B" || node3 == "C"));

    // Remove a node and ensure keys re-map appropriately
    router.removeNode(nodeB);
    std::string node1_after = router.getNode(key1);
    std::string node2_after = router.getNode(key2);
    std::string node3_after = router.getNode(key3);

    // nodeB should no longer be returned
    assert(node1_after != "B");
    assert(node2_after != "B");
    assert(node3_after != "B");

    // All nodes should be either A or C
    assert((node1_after == "A" || node1_after == "C"));
    assert((node2_after == "A" || node2_after == "C"));
    assert((node3_after == "A" || node3_after == "C"));

    // Test empty ring
    ConsistentHashRouter emptyRouter;
    assert(emptyRouter.getNode("anything") == "");

    std::cout << "All tests passed.\n";
    return 0;
}
