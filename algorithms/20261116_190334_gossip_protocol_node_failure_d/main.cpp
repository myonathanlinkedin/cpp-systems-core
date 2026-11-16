#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <chrono>
#include "core.cpp"

int main() {
    using namespace std::chrono;

    // Define timeout of 100 ms
    milliseconds timeout(100);

    // Create nodes with neighbors
    NodeFailureDetector node1(1, {2, 3}, timeout);
    NodeFailureDetector node2(2, {1, 3}, timeout);
    NodeFailureDetector node3(3, {1, 2}, timeout);

    // Initial heartbeats
    node1.sendHeartbeatTo(node2);
    node2.sendHeartbeatTo(node1);
    node3.sendHeartbeatTo(node1);
    node3.sendHeartbeatTo(node2);

    // Allow some time for heartbeats to be processed
    std::this_thread::sleep_for(milliseconds(10));

    // Check failures - none should be detected
    node1.checkFailures();
    node2.checkFailures();
    node3.checkFailures();

    assert(node1.getFailedNodes().empty());
    assert(node2.getFailedNodes().empty());
    assert(node3.getFailedNodes().empty());

    // Simulate node3 failure by not sending heartbeats
    std::this_thread::sleep_for(milliseconds(150));

    // Nodes 1 and 2 should detect node3 as failed
    node1.checkFailures();
    node2.checkFailures();
    node3.checkFailures();

    std::vector<int> failedBy1 = node1.getFailedNodes();
    std::vector<int> failedBy2 = node2.getFailedNodes();
    std::vector<int> failedBy3 = node3.getFailedNodes();

    assert(failedBy1.size() == 1 && failedBy1[0] == 3);
    assert(failedBy2.size() == 1 && failedBy2[0] == 3);
    assert(failedBy3.empty());

    // Node3 should not detect any failures
    assert(failedBy3.empty());

    // Reset failure on node1 and verify
    node1.resetFailure(3);
    node1.checkFailures();
    assert(node1.getFailedNodes().empty());

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
