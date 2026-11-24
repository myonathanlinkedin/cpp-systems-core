#include <string>

#include "core.hpp"
#include <algorithm>

ConsistentHashRouter::ConsistentHashRouter(size_t virtualNodesPerNode)
    : virtualNodesPerNode_(virtualNodesPerNode) {}

void ConsistentHashRouter::addNode(const Node& node) {
    for (size_t i = 0; i < virtualNodesPerNode_; ++i) {
        std::string vnode = node.id + "#" + std::to_string(i);
        std::size_t hash = hasher_(vnode);
        ring_[hash] = node.id;
    }
    nodeIds_.push_back(node.id);
}

void ConsistentHashRouter::removeNode(const Node& node) {
    for (size_t i = 0; i < virtualNodesPerNode_; ++i) {
        std::string vnode = node.id + "#" + std::to_string(i);
        std::size_t hash = hasher_(vnode);
        ring_.erase(hash);
    }
    nodeIds_.erase(std::remove(nodeIds_.begin(), nodeIds_.end(), node.id), nodeIds_.end());
}

std::string ConsistentHashRouter::getNode(const std::string& key) const {
    if (ring_.empty()) return "";
    std::size_t hash = hasher_(key);
    auto it = ring_.lower_bound(hash);
    if (it == ring_.end()) it = ring_.begin();
    return it->second;
}
