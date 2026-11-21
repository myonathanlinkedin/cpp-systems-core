#include <map>
#include <set>
#include <utility>

#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace aho_corasick {

// Node represents a state in the automaton.
struct Node {
    std::unordered_map<char, int> next; // transitions
    int link;                           // suffix link
    std::vector<int> out;               // pattern ids ending here
    Node() : link(-1) {};
};

// Aho-Corasick automaton for multi-pattern string searching.
class AhoCorasick {
public:
    AhoCorasick();
    // Add a pattern with a unique id.
    void addPattern(const std::string& pattern, int id);
    // Build failure links after all patterns are added.
    void build();
    // Search text and return vector of (position, pattern_id) pairs.
    // Position is the index of the last character of the matched pattern.
    std::vector<std::pair<int, int>> search(const std::string& text) const;

private:
    std::vector<Node> nodes_; // root is at index 0
};

} // namespace aho_corasick
