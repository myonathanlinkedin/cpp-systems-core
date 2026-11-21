#include <string>
#include <map>
#include <set>

#include "types.hpp"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace aho_corasick;

int main() {
    // Helper to map pattern id to string for readability.
    std::unordered_map<int, std::string> id_to_pattern;

    // Test 1: Classic example
    {
        AhoCorasick ac;
        std::vector<std::string> patterns = {"he", "she", "his", "hers"};
        for (size_t i = 0; i < patterns.size(); ++i) {
            ac.addPattern(patterns[i], static_cast<int>(i));
            id_to_pattern[static_cast<int>(i)] = patterns[i];
        }
        ac.build();
        std::string text = "ushers";
        auto matches = ac.search(text);
        // Expected matches: ("she" at pos 3), ("he" at pos 4), ("hers" at pos 4)
        std::unordered_map<int, std::vector<int>> expected;
        expected[1] = {3}; // "she"
        expected[0] = {4}; // "he"
        expected[3] = {4}; // "hers"
        std::unordered_map<int, std::vector<int>> actual;
        for (auto& p : matches) {
            actual[p.second].push_back(p.first);
        }
        assert(expected == actual);
    }

    // Test 2: Overlapping patterns
    {
        AhoCorasick ac;
        std::vector<std::string> patterns = {"a", "aa", "aaa"};
        for (size_t i = 0; i < patterns.size(); ++i) {
            ac.addPattern(patterns[i], static_cast<int>(i));
            id_to_pattern[static_cast<int>(i)] = patterns[i];
        }
        ac.build();
        std::string text = "aaaa";
        auto matches = ac.search(text);
        // Count occurrences
        std::unordered_map<int, int> count;
        for (auto& p : matches) count[p.second]++;
        assert(count[0] == 4); // "a" at positions 0,1,2,3
        assert(count[1] == 3); // "aa" at positions 1,2,3
        assert(count[2] == 2); // "aaa" at positions 2,3
    }

    // Test 3: No patterns
    {
        AhoCorasick ac;
        ac.build();
        std::string text = "anything";
        auto matches = ac.search(text);
        assert(matches.empty());
    }

    // Test 4: Pattern longer than text
    {
        AhoCorasick ac;
        ac.addPattern("longpattern", 0);
        ac.build();
        std::string text = "short";
        auto matches = ac.search(text);
        assert(matches.empty());
    }

    // Test 5: Repeated patterns
    {
        AhoCorasick ac;
        ac.addPattern("abc", 0);
        ac.addPattern("abc", 1);
        ac.build();
        std::string text = "abcabc";
        auto matches = ac.search(text);
        // Each occurrence should match both pattern ids
        std::unordered_map<int, int> count;
        for (auto& p : matches) count[p.second]++;
        assert(count[0] == 2);
        assert(count[1] == 2);
    }

    // Test 6: Special characters
    {
        AhoCorasick ac;
        ac.addPattern("c++", 0);
        ac.addPattern("++", 1);
        ac.build();
        std::string text = "c++++";
        auto matches = ac.search(text);
        std::unordered_map<int, std::vector<int>> expected;
        expected[0] = {2}; // "c++" ends at index 2
        expected[1] = {1,3,4}; // "++" ends at indices 1,3,4
        std::unordered_map<int, std::vector<int>> actual;
        for (auto& p : matches) actual[p.second].push_back(p.first);
        assert(expected == actual);
    }

    std::cout << "All tests passed successfully.\n";
    return 0;
}
