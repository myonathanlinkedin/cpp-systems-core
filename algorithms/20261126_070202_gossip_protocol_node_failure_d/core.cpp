#include <map>
#include <set>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <cassert>

struct NodeInfo {
    int ownCounter = 0;          // node's own heartbeat counter
    int lastCounter = 0;         // last counter observed by the system
    int lastSeen = 0;            // last time a heartbeat was received
    bool alive = true;           // currently considered alive
    bool enabled = true;         // whether the node participates in gossip
};

class GossipDetector {
public:
    // failureThreshold : number of ticks without a heartbeat before marking failed
    // gossipPeers      : number of random peers each node gossips to per tick
    // seed             : deterministic RNG seed
    GossipDetector(int failureThreshold, int gossipPeers, unsigned int seed = 42)
        : time_(0),
          failureThreshold_(failureThreshold),
          gossipPeers_(gossipPeers),
          rng_(seed) {};

    // Register a new node with identifier nodeId
    void addNode(int nodeId) {
        assert(nodes_.find(nodeId) == nodes_.end() && "Node already exists");
        NodeInfo info;
        info.lastSeen = time_;
        nodes_.emplace(nodeId, info);
        nodeIds_.push_back(nodeId);
    }

    // Disable a node – it stops sending heartbeats and gossiping
    void disableNode(int nodeId) {
        auto it = nodes_.find(nodeId);
        if (it != nodes_.end()) {
            it->second.enabled = false;
        }
    }

    // Re‑enable a previously disabled node
    void enableNode(int nodeId) {
        auto it = nodes_.find(nodeId);
        if (it != nodes_.end()) {
            it->second.enabled = true;
            // reset its view so it can be considered alive again
            it->second.lastSeen = time_;
            it->second.alive = true;
        }
    }

    // External heartbeat injection (e.g., from a real network). Updates view.
    void receiveHeartbeat(int nodeId, int counter) {
        auto it = nodes_.find(nodeId);
        if (it == nodes_.end()) return;               // unknown node – ignore
        NodeInfo &info = it->second;
        if (counter > info.lastCounter) {
            info.lastCounter = counter;
            info.lastSeen = time_;
            info.alive = true;
        }
    }

    // Advance logical time by one tick, perform gossip and failure detection
    void tick() {
        ++time_;
        // 1) each enabled node increments its own heartbeat counter
        for (int id : nodeIds_) {
            NodeInfo &info = nodes_[id];
            if (!info.enabled) continue;
            ++info.ownCounter;
            // a node always sees its own heartbeat
            info.lastCounter = info.ownCounter;
            info.lastSeen = time_;
        }
        // 2) gossip phase
        gossip();
        // 3) failure detection phase
        detectFailures();
    }

    // Return identifiers of nodes currently marked as failed
    std::vector<int> getFailedNodes() const {
        std::vector<int> failed;
        for (const auto &kv : nodes_) {
            if (!kv.second.alive) failed.push_back(kv.first);
        }
        std::sort(failed.begin(), failed.end());
        return failed;
    }

    // Expose internal state for testing / introspection
    const std::unordered_map<int, NodeInfo>& state() const { return nodes_; }

private:
    int time_;                                 // logical clock
    const int failureThreshold_;               // ticks without heartbeat before failure
    const int gossipPeers_;                    // number of peers each node contacts per tick
    std::mt19937 rng_;                         // deterministic random generator
    std::unordered_map<int, NodeInfo> nodes_; // nodeId -> info
    std::vector<int> nodeIds_;                 // stable list of node identifiers

    // Perform gossip from each enabled node to a random subset of peers
    void gossip() {
        if (nodeIds_.empty()) return;
        std::uniform_int_distribution<std::size_t> dist(0, nodeIds_.size() - 1);
        for (int srcId : nodeIds_) {
            const NodeInfo &srcInfo = nodes_[srcId];
            if (!srcInfo.enabled) continue;
            // select up to gossipPeers_ distinct peers (excluding self)
            std::unordered_set<int> chosen;
            while (static_cast<int>(chosen.size()) < gossipPeers_) {
                int idx = static_cast<int>(dist(rng_));
                int dstId = nodeIds_[idx];
                if (dstId == srcId) continue;
                if (chosen.insert(dstId).second) {
                    // deliver src's heartbeat to dst
                    receiveHeartbeat(srcId, srcInfo.ownCounter);
                }
                if (chosen.size() == static_cast<std::size_t>(nodeIds_.size() - 1)) break;
            }
        }
    }

    // Mark nodes as failed if they have not been seen within the threshold
    void detectFailures() {
        for (auto &kv : nodes_) {
            NodeInfo &info = kv.second;
            if (!info.alive) continue; // already failed
            if (time_ - info.lastSeen > failureThreshold_) {
                info.alive = false;
            }
        }
    }
};
