#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include "core.cpp"   // bring in GossipDetector implementation

void test_basic_operation() {
    GossipDetector detector(3, 2, 123); // failure after 3 ticks, gossip to 2 peers
    detector.addNode(1);
    detector.addNode(2);
    detector.addNode(3);

    // Run a few ticks; no node should be marked failed
    for (int i = 0; i < 5; ++i) detector.tick();
    auto failed = detector.getFailedNodes();
    assert(failed.empty());

    // Disable node 2 and let time pass
    detector.disableNode(2);
    for (int i = 0; i < 5; ++i) detector.tick(); // exceed failureThreshold

    failed = detector.getFailedNodes();
    assert(failed.size() == 1 && failed[0] == 2);
    std::cout << "test_basic_operation passed\n";
}

void test_reenable_node() {
    GossipDetector detector(2, 1, 456);
    detector.addNode(10);
    detector.addNode(20);

    // Normal operation
    detector.tick(); detector.tick();

    // Disable node 20
    detector.disableNode(20);
    detector.tick(); // node 20 missed one heartbeat
    detector.tick(); // node 20 missed two heartbeats -> should be failed
    auto failed = detector.getFailedNodes();
    assert(std::find(failed.begin(), failed.end(), 20) != failed.end());

    // Re‑enable node 20; it should become alive again
    detector.enableNode(20);
    detector.tick(); // node 20 sends a heartbeat now
    failed = detector.getFailedNodes();
    // 20 should no longer appear in the failed list
    assert(std::find(failed.begin(), failed.end(), 20) == failed.end());
    std::cout << "test_reenable_node passed\n";
}

void test_gossip_spread() {
    // Verify that a heartbeat from one node eventually updates all others
    GossipDetector detector(5, 3, 789);
    detector.addNode(100);
    detector.addNode(200);
    detector.addNode(300);
    detector.addNode(400);

    // Manually inject a high counter for node 100
    detector.receiveHeartbeat(100, 999);
    // Run several ticks to allow gossip propagation
    for (int i = 0; i < 10; ++i) detector.tick();

    // All nodes should have seen the high counter (lastSeen updated)
    const auto &state = detector.state();
    for (int id : {100,200,300,400}) {
        const NodeInfo &info = state.at(id);
        assert(info.lastCounter >= 999);
        assert(info.alive);
    }
    std::cout << "test_gossip_spread passed\n";
}

int main() {
    test_basic_operation();
    test_reenable_node();
    test_gossip_spread();
    std::cout << "All tests passed.\n";
    return 0;
}
