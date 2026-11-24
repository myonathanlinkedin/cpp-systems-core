#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include "types.hpp"

class ConsistentHashRouter {
public:
    explicit ConsistentHashRouter(size_t virtualNodesPerNode = 100);
    void addNode(const Node& node);
    void removeNode(const Node& node);
    std::string getNode(const std::string& key) const;
private:
    size_t virtualNodesPerNode_;
    std::map<std::size_t, std::string> ring_;
    std::hash<std::string> hasher_;
    std::vector<std::string> nodeIds_;
};
