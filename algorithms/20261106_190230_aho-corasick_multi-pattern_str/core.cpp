#include <vector>
#include <string>

#include "core.hpp"
#include <queue>
#include <algorithm>

namespace ac {

void AhoCorasick::build(const std::vector<std::string>& patterns) {
    // Reset any previous data.
    nodes_.clear();
    patterns_ = patterns;
    nodes_.emplace_back(); // root node at index 0

    // 1. Insert all patterns into the trie.
    for (int pid = 0; pid < static_cast<int>(patterns.size()); ++pid) {
        const std::string& pat = patterns[pid];
        int v = 0;
        for (char ch : pat) {
            auto it = nodes_[v].next.find(ch);
            if (it == nodes_[v].next.end()) {
                nodes_.emplace_back();
                int new_idx = static_cast<int>(nodes_.size()) - 1;
                nodes_[v].next[ch] = new_idx;
                v = new_idx;
            } else {
                v = it->second;
            }
        }
        nodes_[v].output.push_back(pid);
    }

    // 2. Compute failure links using BFS.
    std::queue<int> q;
    // Initialize root's children.
    for (const auto& kv : nodes_[0].next) {
        int child = kv.second;
        nodes_[child].link = 0;
        q.push(child);
    }
    // Root's failure link is itself.
    nodes_[0].link = 0;

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        // Propagate output links.
        int link = nodes_[v].link;
        if (!nodes_[link].output.empty()) {
            nodes_[v].output.insert(nodes_[v].output.end(),
                                    nodes_[link].output.begin(),
                                    nodes_[link].output.end());
        }

        for (const auto& kv : nodes_[v].next) {
            char ch = kv.first;
            int u = kv.second;
            // Compute failure link for child u.
            int j = nodes_[v].link;
            while (j != 0 && nodes_[j].next.find(ch) == nodes_[j].next.end()) {
                j = nodes_[j].link;
            }
            auto it = nodes_[j].next.find(ch);
            if (it != nodes_[j].next.end() && it->second != u) {
                nodes_[u].link = it->second;
            } else {
                nodes_[u].link = 0;
            }
            q.push(u);
        }
    }
}

int AhoCorasick::go(int v, char c) const {
    while (v != 0 && nodes_[v].next.find(c) == nodes_[v].next.end()) {
        v = nodes_[v].link;
    }
    auto it = nodes_[v].next.find(c);
    if (it != nodes_[v].next.end()) {
        return it->second;
    }
    return 0; // root
}

std::vector<Match> AhoCorasick::search(const std::string& text) const {
    std::vector<Match> result;
    int v = 0; // start at root
    for (std::size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        v = go(v, ch);
        if (!nodes_[v].output.empty()) {
            for (int pid : nodes_[v].output) {
                // Position of the last character of the match.
                result.push_back({pid, i});
            }
        }
    }
    // Optional: sort results for deterministic order (by position then pattern_id).
    std::sort(result.begin(), result.end(),
              [](const Match& a, const Match& b) {
                  if (a.position != b.position) return a.position < b.position;
                  return a.pattern_id < b.pattern_id;
              });
    return result;
}

} // namespace ac
