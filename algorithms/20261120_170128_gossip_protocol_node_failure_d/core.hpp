#include <map>
#include <set>

#pragma once
#include "types.hpp"
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstdint>

namespace gossip {

class FailureDetector {
public:
    explicit FailureDetector(const FailureDetectorConfig& cfg);
    void addNode(const NodeId& nid);
    void removeNode(const NodeId& nid);
    void receiveHeartbeat(const NodeId& from, const NodeId& about);
    void tick(); // advance time, perform gossip and state transitions
    NodeState getState(const NodeId& nid) const;
    std::vector<NodeId> getAllNodes() const;
private:
    FailureDetectorConfig config_;
    std::unordered_map<NodeId, NodeInfo, NodeIdHash> nodes_;
    std::chrono::steady_clock::time_point lastGossip_;
    void gossip();
    void evaluateSuspects();
};

} // namespace gossip
