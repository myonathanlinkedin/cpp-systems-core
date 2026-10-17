#include <vector>
#include <string>
#include <memory>

#pragma once
#include "types.hpp"

namespace lsm {
    class Trie {
    public:
        Trie();
        void insert(const std::string& word, int frequency = 1);
        std::vector<AutoCompleteResult> autocomplete(const std::string& prefix, size_t max_results = 5) const;
    private:
        std::unique_ptr<TrieNode> root_;
        void dfs(const TrieNode* node, std::string current, std::vector<AutoCompleteResult>& results) const;
    };
}
