#include <map>
#include <set>

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace lsm {
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool is_word = false;
        int frequency = 0;
        TrieNode() = default;
    };

    struct AutoCompleteResult {
        std::string word;
        int frequency;
    };
}
