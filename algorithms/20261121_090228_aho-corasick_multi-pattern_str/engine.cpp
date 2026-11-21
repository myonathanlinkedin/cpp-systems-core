#include <vector>
#include <string>
#include <utility>

#include "types.hpp"
#include <queue>
#include <algorithm>

namespace aho_corasick {

AhoCorasick::AhoCorasick() {
    nodes_.push_back(Node()); // root
}

void AhoCorasick::addPattern(const std::string& pattern, int id) {
    int current = 0;
    for (char ch : pattern) {
        auto it = nodes_[current].next.find(ch);
        if (it == nodes_[current].next.end()) {
            nodes_[current].next[ch] = static_cast<int>(nodes_.size());
            nodes_.push_back(Node());
            current = nodes_[current].next[ch];
        } else {
            current = it->second;
        }
    }
    nodes_[current].out.push_back(id);
}

void AhoCorasick::build() {
    std::queue<int> q;
    // Initialize links of depth-1 nodes to root.
    for (const auto& pair : nodes_[0].next) {
        int child = pair.second;
        nodes_[child].link = 0;
        q.push(child);
    }

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (const auto& pair : nodes_[v].next) {
            char ch = pair.first;
            int u = pair.second;
            int link = nodes_[v].link;
            while (link != 0 && nodes_[link].next.find(ch) == nodes_[link].next.end()) {
                link = nodes_[link].link;
            }
            if (nodes_[link].next.find(ch) != nodes_[link].next.end()) {
                link = nodes_[link].next[ch];
            }
            nodes_[u].link = link;
            // Merge output lists
            nodes_[u].out.insert(nodes_[u].out.end(),
                                 nodes_[link].out.begin(),
                                 nodes_[link].out.end());
            q.push(u);
        }
    }
}

std::vector<std::pair<int, int>> AhoCorasick::search(const std::string& text) const {
    std::vector<std::pair<int, int>> result;
    int state = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        while (state != 0 && nodes_[state].next.find(ch) == nodes_[state].next.end()) {
            state = nodes_[state].link;
        }
        if (nodes_[state].next.find(ch) != nodes_[state].next.end()) {
            state = nodes_[state].next.at(ch);
        }
        for (int pat_id : nodes_[state].out) {
            result.emplace_back(static_cast<int>(i), pat_id);
        }
    }
    return result;
}

} // namespace aho_corasick
