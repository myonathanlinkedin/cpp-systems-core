#include <vector>
#include <string>

#include "types.hpp"
#include <queue>
#include <algorithm>

namespace aho {

AhoCorasick::AhoCorasick() {
    // Create root node (index 0)
    nodes_.emplace_back();
    nodes_[0].link = 0;
}

void AhoCorasick::addPattern(const std::string& pattern, int id) {
    int v = 0;
    for (char ch : pattern) {
        auto it = nodes_[v].next.find(ch);
        if (it == nodes_[v].next.end()) {
            nodes_.emplace_back();
            int newNode = static_cast<int>(nodes_.size()) - 1;
            nodes_[v].next[ch] = newNode;
            v = newNode;
        } else {
            v = it->second;
        }
    }
    nodes_[v].output.push_back(id);
}

void AhoCorasick::build() {
    std::queue<int> q;

    // Initialize depth‑1 nodes: failure link points to root
    for (const auto& p : nodes_[0].next) {
        int child = p.second;
        nodes_[child].link = 0;
        q.push(child);
    }

    // BFS over the trie
    while (!q.empty()) {
        int v = q.front(); q.pop();

        for (const auto& p : nodes_[v].next) {
            char ch = p.first;
            int u = p.second;

            int link = nodes_[v].link;
            while (link != 0 && nodes_[link].next.find(ch) == nodes_[link].next.end()) {
                link = nodes_[link].link;
            }
            if (nodes_[link].next.find(ch) != nodes_[link].next.end()) {
                link = nodes_[link].next.at(ch);
            }
            nodes_[u].link = link;

            // Merge output of the failure node
            const auto& outLink = nodes_[link].output;
            nodes_[u].output.insert(nodes_[u].output.end(),
                                    outLink.begin(), outLink.end());

            q.push(u);
        }
    }
}

std::vector<Match> AhoCorasick::search(const std::string& text) const {
    std::vector<Match> result;
    int v = 0;

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];

        while (v != 0 && nodes_[v].next.find(ch) == nodes_[v].next.end()) {
            v = nodes_[v].link;
        }

        auto it = nodes_[v].next.find(ch);
        if (it != nodes_[v].next.end()) {
            v = it->second;
        }

        // Emit all patterns that end at this position
        for (int pid : nodes_[v].output) {
            result.emplace_back(pid, i);
        }
    }
    return result;
}

} // namespace aho
