#include <vector>
#include <string>

#include "types.hpp"
#include <algorithm>
#include <numeric>
#include <cassert>

namespace storj {

std::vector<Shard> splitObjectIntoShards(const Object& obj, std::size_t shard_count) {
    assert(shard_count > 0 && "Shard count must be positive");
    std::vector<Shard> result;
    result.reserve(shard_count);
    std::size_t total_size = obj.data.size();
    std::size_t base_size = total_size / shard_count;
    std::size_t remainder = total_size % shard_count;

    std::size_t offset = 0;
    for (std::size_t i = 0; i < shard_count; ++i) {
        std::size_t cur_size = base_size + (i < remainder ? 1 : 0);
        std::vector<char> fragment(obj.data.begin() + offset,
                                   obj.data.begin() + offset + cur_size);
        result.emplace_back(static_cast<int>(i), obj.key, std::move(fragment));
        offset += cur_size;
    }
    return result;
}

void distributeShards(const std::vector<Shard>& shards, std::vector<Node>& nodes) {
    assert(!nodes.empty() && "At least one node required for distribution");
    // Round‑robin placement of shards across nodes.
    for (std::size_t i = 0; i < shards.size(); ++i) {
        Node& target = nodes[i % nodes.size()];
        target.shards[shards[i].parent_key].push_back(shards[i]);
    }
}

std::optional<Object> retrieveObject(const std::string& key, const std::vector<Node>& nodes) {
    // Collect all shards for the given key from every node.
    std::vector<Shard> collected;
    for (const auto& node : nodes) {
        auto it = node.shards.find(key);
        if (it != node.shards.end()) {
            collected.insert(collected.end(), it->second.begin(), it->second.end());
        }
    }
    if (collected.empty()) {
        return std::nullopt; // object not found
    }

    // Verify that we have a contiguous set of shard ids starting at 0.
    std::sort(collected.begin(), collected.end(),
              [](const Shard& a, const Shard& b) { return a.shard_id < b.shard_id; });

    for (std::size_t i = 0; i < collected.size(); ++i) {
        if (collected[i].shard_id != static_cast<int>(i)) {
            return std::nullopt; // missing or duplicate shard
        }
    }

    // Reassemble data.
    std::vector<char> full_data;
    std::size_t total_len = std::accumulate(collected.begin(), collected.end(), std::size_t(0),
                                            [](std::size_t sum, const Shard& s) { return sum + s.data.size(); });
    full_data.reserve(total_len);
    for (const auto& shard : collected) {
        full_data.insert(full_data.end(), shard.data.begin(), shard.data.end());
    }
    return Object(key, std::move(full_data));
}

} // namespace storj
