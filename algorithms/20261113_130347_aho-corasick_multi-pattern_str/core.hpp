#include <unordered_map>
#include <map>
#include <set>

#pragma once
#include <string>
#include <vector>
#include "types.hpp"

namespace ac {

class AhoCorasick {
public:
    AhoCorasick() = default;

    // Insert a pattern with a user‑provided identifier.
    void addPattern(const std::string& pattern, int id);

    // Build failure links; must be called before search().
    void build();

    // Search the text and return all matches (pattern id, ending position).
    std::vector<Match> search(const std::string& text) const;

private:
    struct Node {
        std::unordered_map<char, int> next; // transitions
        int link = -1;                       // failure link
        std::vector<int> out;                // pattern ids ending here
        Node() = default;
    };

    std::vector<Node> trie_;
    bool built_ = false;

    void ensureRoot();
};

} // namespace ac
