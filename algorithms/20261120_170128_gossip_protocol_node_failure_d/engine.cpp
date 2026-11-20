#include <vector>
#include <chrono>

#include "core.hpp"
#include <random>
#include <algorithm>

namespace gossip {

FailureDetector::FailureDetector(const FailureDetectorConfig& cfg)
    : config_(cfg), lastGossip_(std::chrono::steady_clock::now())
{
    // empty
}

void FailureDetector::addNode(const NodeId& nid) {
    if (nodes_.find(nid) == nodes_.end()) {
        NodeInfo info;
        info.lastHeartbeat = std::chrono::steady_clock::now();
        nodes_.emplace(nid, std::move(info));
    }
}

void FailureDetector::removeNode(const NodeId& nid) {
    nodes_.erase(nid);
}

void FailureDetector::receiveHeartbeat(const NodeId& from, const NodeId& about) {
    auto now = std::chrono::steady_clock::now();
    // Update sender's liveness
    auto itFrom = nodes_.find(from);
    if (itFrom != nodes_.end()) {
        itFrom->second.lastHeartbeat = now;
        if (itFrom->second.state != NodeState::Alive) {
            itFrom->second.state = NodeState::Alive;
        }
    }
    // Update the node about which the heartbeat is about
    auto it = nodes_.find(about);
    if (it != nodes_.end()) {
        it->second.lastHeartbeat = now;
        if (it->second.state != NodeState::Alive) {
            it->second.state = NodeState::Alive;
        }
    }
}

void FailureDetector::tick() {
    auto now = std::chrono::steady_clock::now();
    if (now - lastGossip_ >= config_.gossipInterval) {
        gossip();
        lastGossip_ = now;
    }
    evaluateSuspects();
}

void FailureDetector::gossip() {
    if (nodes_.empty()) return;
    // Collect node IDs
    std::vector<NodeId> ids;
    ids.reserve(nodes_.size());
    for (const auto& kv : nodes_) ids.push_back(kv.first);
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, ids.size() - 1);
    // Choose source and destination
    NodeId src = ids[dist(rng)];
    NodeId dst;
    do {
        dst = ids[dist(rng)];
    } while (dst == src && ids.size() > 1);
    // Simulate heartbeat about src to dst
    receiveHeartbeat(src, src);
    // Gossip about a random other node
    NodeId other = ids[dist(rng)];
    receiveHeartbeat(src, other);
    (void)dst; // dst is not used further; placeholder for realistic network send
}

void FailureDetector::evaluateSuspects() {
    auto now = std::chrono::steady_clock::now();
    for (auto& kv : nodes_) {
        auto& info = kv.second;
        if (info.state == NodeState::Alive) {
            if (now - info.lastHeartbeat > config_.suspectTimeout) {
                info.state = NodeState::Suspect;
                info.suspectSince = now;
            }
        } else if (info.state == NodeState::Suspect) {
            if (now - info.suspectSince > config_.cleanupTimeout) {
                info.state = NodeState::Dead;
            }
        }
    }
}

NodeState FailureDetector::getState(const NodeId& nid) const {
    auto it = nodes_.find(nid);
    if (it == nodes_.end()) return NodeState::Dead;
    return it->second.state;
}

std::vector<NodeId> FailureDetector::getAllNodes() const {
    std::vector<NodeId> result;
    result.reserve(nodes_.size());
    for (const auto& kv : nodes_) result.push_back(kv.first);
    return result;
}

} // namespace gossip
