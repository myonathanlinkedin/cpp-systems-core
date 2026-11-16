#include <map>
#include <set>

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace aho {

struct Match {
    int pattern_id;
    size_t position; // index of the last character of the match (0‑based)

    Match() = default;
    Match(int pid, size_t pos) : pattern_id(pid), position(pos) {};
};

struct Node {
    std::unordered_map<char, int> next;   // transition edges
    int link;                             // failure link
    std::vector<int> output;              // pattern ids ending at this node

    Node() : link(-1) {};
};

class AhoCorasick {
public:
    AhoCorasick();

    // Add a pattern with a unique identifier (must be non‑negative)
    void addPattern(const std::string& pattern, int id);

    // After all patterns are added, compute failure links and output links
    void build();

    // Search the text and return all matches (pattern id, ending position)
    std::vector<Match> search(const std::string& text) const;

private:
    std::vector<Node> nodes_;
};

} // namespace aho
