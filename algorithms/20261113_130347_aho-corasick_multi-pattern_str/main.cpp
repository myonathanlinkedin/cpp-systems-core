#include <utility>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

using namespace ac;

void test_basic() {
    AhoCorasick ac;
    ac.addPattern("he", 0);
    ac.addPattern("she", 1);
    ac.addPattern("his", 2);
    ac.addPattern("hers", 3);
    ac.build();

    std::string text = "ushers";
    auto matches = ac.search(text);

    // Expected: "she" (id 1) ends at 3, "he" (id 0) ends at 3, "hers" (id 3) ends at 5
    std::vector<std::pair<int, std::size_t>> expected = {
        {1, 3}, {0, 3}, {3, 5}
    };
    assert(matches.size() == expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        assert(matches[i].patternId == expected[i].first);
        assert(matches[i].position == expected[i].second);
    }
}

void test_overlap() {
    AhoCorasick ac;
    ac.addPattern("a", 0);
    ac.addPattern("aa", 1);
    ac.addPattern("aaa", 2);
    ac.build();

    std::string text = "aaaa";
    auto matches = ac.search(text);

    // All matches (pattern id, ending index)
    std::vector<std::pair<int, std::size_t>> expected = {
        {0,0},{0,1},{1,1},{0,2},{1,2},{2,2},{0,3},{1,3},{2,3}
    };
    assert(matches.size() == expected.size());

    // Sort for deterministic comparison.
    std::sort(matches.begin(), matches.end(),
        [](const Match& a, const Match& b){
            return a.position < b.position ||
                   (a.position == b.position && a.patternId < b.patternId);
        });
    std::sort(expected.begin(), expected.end(),
        [](auto a, auto b){
            return a.second < b.second ||
                   (a.second == b.second && a.first < b.first);
        });

    for (size_t i = 0; i < expected.size(); ++i) {
        assert(matches[i].patternId == expected[i].first);
        assert(matches[i].position == expected[i].second);
    }
}

void test_no_patterns() {
    AhoCorasick ac;
    ac.build(); // empty automaton
    std::string text = "anything";
    auto matches = ac.search(text);
    assert(matches.empty());
}

void test_utf8_bytes() {
    AhoCorasick ac;
    ac.addPattern("😀", 0); // each UTF‑8 byte is treated as a separate char
    ac.build();

    std::string text = "😀😀";
    auto matches = ac.search(text);
    // Emoji is three bytes; matches end at byte indices 2 and 5.
    assert(matches.size() == 2);
    assert(matches[0].position == 2);
    assert(matches[1].position == 5);
}

int main() {
    test_basic();
    test_overlap();
    test_no_patterns();
    test_utf8_bytes();
    std::cout << "All Aho‑Corasick tests passed.\n";
    return 0;
}
