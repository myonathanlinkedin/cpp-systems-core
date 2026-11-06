#include <string>

#include <iostream>
#include <vector>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

using namespace ac;

void test_basic() {
    std::vector<std::string> pats = {"he", "she", "his", "hers"};
    AhoCorasick automaton;
    automaton.build(pats);
    std::string text = "ushers";
    auto matches = automaton.search(text);
    // Expected matches:
    // "she" at positions 2-4 (end index 4)
    // "he" at positions 3-4 (end index 4)
    // "hers" at positions 3-6 (end index 6)
    std::vector<Match> expected = {
        {1, 4}, // "she"
        {0, 4}, // "he"
        {3, 6}  // "hers"
    };
    assert(matches == expected);
}

void test_overlap() {
    std::vector<std::string> pats = {"a", "ab", "bab", "bc", "bca", "c", "caa"};
    AhoCorasick automaton;
    automaton.build(pats);
    std::string text = "abccab";
    auto matches = automaton.search(text);
    std::vector<Match> expected = {
        {0,0}, // "a" at 0
        {1,1}, // "ab" at 1
        {5,2}, // "c" at 2
        {5,3}, // "c" at 3
        {0,4}, // "a" at 4
        {1,5}  // "ab" at 5
    };
    assert(matches == expected);
}

void test_no_match() {
    std::vector<std::string> pats = {"xyz", "123"};
    AhoCorasick automaton;
    automaton.build(pats);
    std::string text = "abcdef";
    auto matches = automaton.search(text);
    assert(matches.empty());
}

void test_empty_patterns() {
    std::vector<std::string> pats;
    AhoCorasick automaton;
    automaton.build(pats);
    std::string text = "anything";
    auto matches = automaton.search(text);
    assert(matches.empty());
}

void test_duplicate_patterns() {
    std::vector<std::string> pats = {"test", "test", "te"};
    AhoCorasick automaton;
    automaton.build(pats);
    std::string text = "test";
    auto matches = automaton.search(text);
    // Both "test" entries should be reported, plus "te".
    std::vector<Match> expected = {
        {2,1}, // "te" ends at index 1
        {0,3}, // first "test"
        {1,3}  // second "test"
    };
    assert(matches == expected);
}

int main() {
    test_basic();
    test_overlap();
    test_no_match();
    test_empty_patterns();
    test_duplicate_patterns();

    std::cout << "All Aho‑Corasick unit tests passed.\n";
    return 0;
}
