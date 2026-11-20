#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

using namespace gossip;

int main() {
    FailureDetectorConfig cfg;
    cfg.gossipInterval = std::chrono::milliseconds(10);
    cfg.suspectTimeout = std::chrono::milliseconds(30);
    cfg.cleanupTimeout = std::chrono::milliseconds(100);
    FailureDetector fd(cfg);

    NodeId n1{1}, n2{2}, n3{3};
    fd.addNode(n1);
    fd.addNode(n2);
    fd.addNode(n3);

    // Initially all alive
    assert(fd.getState(n1) == NodeState::Alive);
    assert(fd.getState(n2) == NodeState::Alive);
    assert(fd.getState(n3) == NodeState::Alive);

    // Simulate heartbeats for n1 and n2, but not n3
    fd.receiveHeartbeat(n1, n1);
    fd.receiveHeartbeat(n2, n2);

    // Advance time without n3 heartbeat
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    fd.tick(); // should suspect n3
    assert(fd.getState(n3) == NodeState::Suspect);
    // n1 and n2 remain alive
    assert(fd.getState(n1) == NodeState::Alive);
    assert(fd.getState(n2) == NodeState::Alive);

    // Wait beyond cleanup timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    fd.tick(); // should mark n3 dead
    assert(fd.getState(n3) == NodeState::Dead);

    // Verify that receiving heartbeat revives node
    fd.receiveHeartbeat(n3, n3);
    assert(fd.getState(n3) == NodeState::Alive);

    // Test removal
    fd.removeNode(n2);
    assert(fd.getState(n2) == NodeState::Dead);
    // Ensure remaining nodes still functional
    fd.tick();
    assert(fd.getState(n1) == NodeState::Alive);
    assert(fd.getState(n3) == NodeState::Alive);

    // Stress test: rapid ticks and random gossip
    for (int i = 0; i < 1000; ++i) {
        fd.tick();
    }

    std::cout << "All tests passed.\n";
    return 0;
}
