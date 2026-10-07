#include <map>
#include <set>

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstddef>

namespace storj {

// Simple representation of an object stored in the system.
struct Object {
    std::string key;
    std::vector<char> data;
    Object() = default;
    Object(std::string k, std::vector<char> d) : key(std::move(k)), data(std::move(d)) {};
};

// A shard is a fragment of an object.
struct Shard {
    int shard_id = 0;                     // sequential id within the object
    std::string parent_key;               // key of the original object
    std::vector<char> data;               // fragment data
    Shard() = default;
    Shard(int id, std::string key, std::vector<char> d)
        : shard_id(id), parent_key(std::move(key)), data(std::move(d)) {};
};

// A storage node in the decentralized network.
struct Node {
    int node_id = 0;
    // Mapping from object key to the list of shards stored on this node.
    std::unordered_map<std::string, std::vector<Shard>> shards;
    Node() = default;
    explicit Node(int id) : node_id(id) {};
};

} // namespace storj

// Forward declarations of core engine functions.
namespace storj {
    std::vector<Shard> splitObjectIntoShards(const Object& obj, std::size_t shard_count);
    void distributeShards(const std::vector<Shard>& shards, std::vector<Node>& nodes);
    std::optional<Object> retrieveObject(const std::string& key, const std::vector<Node>& nodes);
}
