#include <vector>
#include <string>
#include <utility>

#include "core.hpp"
#include <algorithm>
#include <iostream>

namespace lsm {

LSMTree::LSMTree(size_t memtable_threshold)
    : memtable_threshold_(memtable_threshold) {}

void LSMTree::put(const std::string& key, const std::string& value) {
    memtable_[key] = value;
    if (memtable_.size() >= memtable_threshold_) {
        flush();
    }
}

std::optional<std::string> LSMTree::get(const std::string& key) const {
    auto it = memtable_.find(key);
    if (it != memtable_.end()) {
        return it->second;
    }
    for (auto rit = sstables_.rbegin(); rit != sstables_.rend(); ++rit) {
        const auto& table = *rit;
        auto comp = [](const std::pair<std::string, std::string>& a, const std::string& b) {
            return a.first < b;
        };
        auto lb = std::lower_bound(table.begin(), table.end(), key, comp);
        if (lb != table.end() && lb->first == key) {
            return lb->second;
        }
    }
    return std::nullopt;
}

void LSMTree::flush() {
    std::vector<std::pair<std::string, std::string>> table;
    table.reserve(memtable_.size());
    for (const auto& kv : memtable_) {
        table.emplace_back(kv.first, kv.second);
    }
    std::sort(table.begin(), table.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    sstables_.push_back(std::move(table));
    memtable_.clear();
}

void LSMTree::compact() {
    if (sstables_.empty()) return;
    std::vector<std::pair<std::string, std::string>> all;
    for (const auto& table : sstables_) {
        all.insert(all.end(), table.begin(), table.end());
    }
    std::sort(all.begin(), all.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    std::vector<std::pair<std::string, std::string>> merged;
    for (auto it = all.rbegin(); it != all.rend(); ++it) {
        if (merged.empty() || merged.back().first != it->first) {
            merged.push_back(*it);
        }
    }
    std::reverse(merged.begin(), merged.end());
    sstables_.clear();
    sstables_.push_back(std::move(merged));
}

} // namespace lsm
