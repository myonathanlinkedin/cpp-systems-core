#include <vector>
#include <string>
#include <utility>

#include "types.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>

int main() {
    using namespace aho;

    // Test 1: basic example from classic Aho‑Corasick literature
    {
        AhoCorasick ac;
        ac.addPattern("he",   0);
        ac.addPattern("she",  1);
        ac.addPattern("his",  2);
        ac.addPattern("hers", 3);
        ac.build();

        std::string text = "ushers";
        auto matches = ac.search(text);

        // Expected: "she" (id 1) ends at 3, "he" (id 0) ends at 3, "hers" (id 3) ends at 4
        std::vector<std::pair<int, size_t>> expected = {
            {1, 3}, {0, 3}, {3, 4}
        };
        assert(matches.size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            assert(matches[i].pattern_id == expected[i].first);
            assert(matches[i].position   == expected[i].second);
        }
    }

    // Test 2: overlapping patterns
    {
        AhoCorasick ac;
        ac.addPattern("a",   0);
        ac.addPattern("ab",  1);
        ac.addPattern("bab", 2);
        ac.addPattern("bc",  3);
        ac.addPattern("bca", 4);
        ac.addPattern("c",   5);
        ac.build();

        std::string text = "ababc";
        auto matches = ac.search(text);

        // Expected matches (pattern_id, end_position):
        // a@0, ab@1, a@2, ab@3, c@4
        std::vector<std::pair<int, size_t>> expected = {
            {0,0}, {1,1}, {0,2}, {1,3}, {5,4}
        };
        assert(matches.size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            assert(matches[i].pattern_id == expected[i].first);
            assert(matches[i].position   == expected[i].second);
        }
    }

    // Test 3: no matches at all
    {
        AhoCorasick ac;
        ac.addPattern("xyz", 0);
        ac.build();

        std::string text = "abcdef";
        auto matches = ac.search(text);
        assert(matches.empty());
    }

    // Test 4: empty pattern set (search must be safe and return empty)
    {
        AhoCorasick ac;
        ac.build();

        std::string text = "anything";
        auto matches = ac.search(text);
        assert(matches.empty());
    }

    // Test 5: deterministic larger test
    {
        AhoCorasick ac;
        std::vector<std::string> pats = {"ab", "bc", "abc", "c"};
        for (size_t i = 0; i < pats.size(); ++i) {
            ac.addPattern(pats[i], static_cast<int>(i));
        }
        ac.build();

        std::string text = "abcabc";
        auto matches = ac.search(text);

        // Expected matches:
        // "ab"(0)@1, "bc"(1)@2, "abc"(2)@2, "c"(3)@2,
        // "ab"(0)@4, "bc"(1)@5, "abc"(2)@5, "c"(3)@5
        std::vector<std::pair<int, size_t>> expected = {
            {0,1},{1,2},{2,2},{3,2},
            {0,4},{1,5},{2,5},{3,5}
        };
        assert(matches.size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            assert(matches[i].pattern_id == expected[i].first);
            assert(matches[i].position   == expected[i].second);
        }
    }

    std::cout << "All Aho‑Corasick tests passed.\n";
    return 0;
}
