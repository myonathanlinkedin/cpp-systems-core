#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "core.hpp"

namespace lsm {
    Trie::Trie() : root_(std::make_unique<TrieNode>()) {}

    void Trie::insert(const std::string& word, int frequency) {
        TrieNode* node = root_.get();
        for (char c : word) {
            auto it = node->children.find(c);
            if (it == node->children.end()) {
                node->children[c] = std::make_unique<TrieNode>();
            }
            node = node->children[c].get();
        }
        node->is_word = true;
        node->frequency += frequency;
    }

    void Trie::dfs(const TrieNode* node, std::string current, std::vector<AutoCompleteResult>& results) const {
        if (!node) return;
        if (node->is_word) {
            results.push_back({current, node->frequency});
        }
        for (const auto& pair : node->children) {
            dfs(pair.second.get(), current + pair.first, results);
        }
    }

    std::vector<AutoCompleteResult> Trie::autocomplete(const std::string& prefix, size_t max_results) const {
        const TrieNode* node = root_.get();
        for (char c : prefix) {
            auto it = node->children.find(c);
            if (it == node->children.end()) {
                return {}; // no matches
            }
            node = it->second.get();
        }
        std::vector<AutoCompleteResult> results;
        dfs(node, prefix, results);
        std::sort(results.begin(), results.end(), [](const AutoCompleteResult& a, const AutoCompleteResult& b) {
            if (a.frequency != b.frequency) return a.frequency > b.frequency;
            return a.word < b.word;
        });
        if (results.size() > max_results) {
            results.resize(max_results);
        }
        return results;
    }
}
