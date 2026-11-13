#include <vector>
#include <string>
#include <cassert>

#include "core.hpp"
#include <queue>
#include <algorithm>

namespace ac {

void AhoCorasick::ensureRoot() {
    if (trie_.empty()) {
        trie_.emplace_back();          // root node index 0
        trie_[0].link = 0;
    }
}

void AhoCorasick::addPattern(const std::string& pattern, int id) {
    ensureRoot();
    int v = 0;
    for (char ch : pattern) {
        auto it = trie_[v].next.find(ch);
        if (it == trie_[v].next.end()) {
            trie_[v].next[ch] = static_cast<int>(trie_.size());
            trie_.emplace_back();
        }
        v = trie_[v].next[ch];
    }
    trie_[v].out.push_back(id);
    built_ = false;
}

void AhoCorasick::build() {
    ensureRoot();
    std::queue<int> q;

    // Initialize depth‑1 nodes.
    for (auto& p : trie_[0].next) {
        int child = p.second;
        trie_[child].link = 0;
        q.push(child);
    }

    // BFS over the trie.
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (auto& p : trie_[v].next) {
            char ch = p.first;
            int u = p.second;

            int link = trie_[v].link;
            while (link != 0 && trie_[link].next.find(ch) == trie_[link].next.end()) {
                link = trie_[link].link;
            }
            if (trie_[link].next.find(ch) != trie_[link].next.end()) {
                link = trie_[link].next[ch];
            }
            trie_[u].link = link;

            // Merge output lists.
            const auto& outLink = trie_[link].out;
            trie_[u].out.insert(trie_[u].out.end(), outLink.begin(), outLink.end());

            q.push(u);
        }
    }
    built_ = true;
}

std::vector<Match> AhoCorasick::search(const std::string& text) const {
    assert(built_ && "Automaton must be built before searching");
    std::vector<Match> result;
    int v = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        while (v != 0 && trie_[v].next.find(ch) == trie_[v].next.end()) {
            v = trie_[v].link;
        }
        auto it = trie_[v].next.find(ch);
        if (it != trie_[v].next.end()) {
            v = it->second;
        }
        for (int pid : trie_[v].out) {
            result.push_back({pid, i});
        }
    }
    return result;
}

} // namespace ac
