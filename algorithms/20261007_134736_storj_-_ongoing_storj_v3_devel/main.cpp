#include <vector>
#include <string>

#include "types.hpp"
#include "engine.cpp"
#include <iostream>
#include <cassert>

int main() {
    using namespace storj;

    // Test data preparation.
    const std::string test_key = "sample_object";
    const std::string payload = "The quick brown fox jumps over the lazy dog.";
    Object original(test_key, std::vector<char>(payload.begin(), payload.end()));

    // 1. Split into shards.
    constexpr std::size_t shard_count = 5;
    auto shards = splitObjectIntoShards(original, shard_count);
    assert(shards.size() == shard_count);
    // Verify each shard's parent key matches.
    for (const auto& sh : shards) {
        assert(sh.parent_key == test_key);
    }

    // 2. Create nodes and distribute shards.
    std::vector<Node> nodes;
    for (int i = 0; i < 3; ++i) {
        nodes.emplace_back(i);
    }
    distributeShards(shards, nodes);
    // Ensure each node has at least one shard.
    bool any_node_has_shard = std::any_of(nodes.begin(), nodes.end(),
                                          [&](const Node& n){ return !n.shards.empty(); });
    assert(any_node_has_shard);

    // 3. Retrieve the object.
    auto retrieved_opt = retrieveObject(test_key, nodes);
    assert(retrieved_opt.has_value());
    const Object& retrieved = retrieved_opt.value();
    assert(retrieved.key == original.key);
    assert(retrieved.data == original.data);

    // 4. Edge case: request non‑existent object.
    auto missing = retrieveObject("nonexistent_key", nodes);
    assert(!missing.has_value());

    // 5. Edge case: incomplete shard set.
    // Remove a shard from all nodes.
    for (auto& node : nodes) {
        auto it = node.shards.find(test_key);
        if (it != node.shards.end() && !it->second.empty()) {
            it->second.pop_back(); // remove last shard from this node
        }
    }
    auto incomplete = retrieveObject(test_key, nodes);
    assert(!incomplete.has_value());

    std::cout << "All Storj simulation tests passed.\n";
    return 0;
}
