#include <map>
#include <set>

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "types.hpp"

namespace ac {

class AhoCorasick {
public:
    // Constructs an empty automaton.
    AhoCorasick() = default;

    // Builds the automaton from the given list of patterns.
    // The order of patterns determines their pattern_id (0‑based).
    void build(const std::vector<std::string>& patterns);

    // Searches the given text and returns all matches.
    // Each match reports the pattern_id and the index of the last character.
    std::vector<Match> search(const std::string& text) const;

private:
    struct Node {
        // Transitions for characters that exist directly from this node.
        std::unordered_map<char, int> next;
        // Failure link (the longest proper suffix that is also a prefix).
        int link = -1;
        // List of pattern ids that end at this node.
        std::vector<int> output;
        Node() = default;
    };

    std::vector<Node> nodes_;          // Trie nodes; node 0 is the root.
    std::vector<std::string> patterns_; // Stored patterns for reference (optional).

    // Helper to get the transition for character c from node v,
    // following failure links when necessary.
    int go(int v, char c) const;
};

} // namespace ac
