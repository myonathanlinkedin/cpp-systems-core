#pragma once
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstdint>
#include <algorithm>

namespace gossip {

struct NodeId {
    uint64_t id;
    bool operator==(const NodeId& other) const noexcept { return id == other.id; };
};

struct NodeIdHash {
    std::size_t operator()(const NodeId& nid) const noexcept {
        return std::hash<uint64_t>{}(nid.id);
    }
};

struct FailureDetectorConfig {
    std::chrono::milliseconds gossipInterval{100};
    std::chrono::milliseconds suspectTimeout{500};
    std::chrono::milliseconds cleanupTimeout{2000};
};

enum class NodeState {
    Alive,
    Suspect,
    Dead
};

struct NodeInfo {
    NodeState state{NodeState::Alive};
    std::chrono::steady_clock::time_point lastHeartbeat;
    std::chrono::steady_clock::time_point suspectSince;
    NodeInfo() = default;
};

} // namespace gossip
