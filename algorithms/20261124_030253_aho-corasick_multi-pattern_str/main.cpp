#include <unordered_map>
#include <map>
#include <set>
#include <utility>

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <unordered_set>
#include "core.cpp"

using namespace aho;

static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond) do { \
    if (cond) { ++g_pass; } \
    else { ++g_fail; std::cerr << "FAIL at line " << __LINE__ << ": " #cond << "\n"; } \
} while (0)

static std::vector<std::pair<int,int>> collectEnds(const std::vector<std::pair<int,int>>& hits) {
    std::vector<std::pair<int,int>> v;
    for (auto& h : hits) v.push_back(h);
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    return v;
}

int main() {
    // Test 1: single pattern
    {
        AhoCorasick ac;
        ac.addPattern("he");
        ac.build();
        auto hits = ac.search("she sells sea shells");
        CHECK(hits.size() == 1);
        if (!hits.empty()) {
            CHECK(hits[0].second == 1); // end index of "he" in "she"
        }
    }

    // Test 2: multiple patterns, overlapping
    {
        AhoCorasick ac;
        ac.addPattern("he");
        ac.addPattern("she");
        ac.addPattern("his");
        ac.addPattern("hers");
        ac.build();
        auto hits = ac.search("ushers");
        // "she" ends at index 3, "he" ends at index 3, "hers" ends at index 5
        std::unordered_set<int> ends;
        for (auto& h : hits) ends.insert(h.second);
        CHECK(ends.count(3) == 1);
        CHECK(ends.count(5) == 1);
        CHECK(hits.size() >= 3);
    }

    // Test 3: no match
    {
        AhoCorasick ac;
        ac.addPattern("abc");
        ac.build();
        auto hits = ac.search("xyz");
        CHECK(hits.empty());
    }

    // Test 4: pattern at start
    {
        AhoCorasick ac;
        ac.addPattern("ab");
        ac.build();
        auto hits = ac.search("abab");
        CHECK(hits.size() == 2);
    }

    // Test 5: empty text
    {
        AhoCorasick ac;
        ac.addPattern("a");
        ac.build();
        auto hits = ac.search("");
        CHECK(hits.empty());
    }

    // Test 6: single char patterns
    {
        AhoCorasick ac;
        ac.addPattern("a");
        ac.addPattern("b");
        ac.build();
        auto hits = ac.search("abab");
        CHECK(hits.size() == 4);
    }

    // Test 7: overlapping via fail links (classic)
    {
        AhoCorasick ac;
        ac.addPattern("a");
        ac.addPattern("aa");
        ac.addPattern("aaa");
        ac.build();
        auto hits = ac.search("aaaa");
        // positions: a at 0,1,2,3; aa at 1,2,3; aaa at 2,3 => total 4+3+2=9
        CHECK(hits.size() == 9);
    }

    // Test 8: many patterns performance sanity
    {
        AhoCorasick ac;
        for (int i = 0; i < 100; ++i) {
            ac.addPattern("p" + std::to_string(i));
        }
        ac.build();
        std::string text;
        for (int i = 0; i < 100; ++i) text += "p" + std::to_string(i) + " ";
        auto hits = ac.search(text);
        CHECK(hits.size() == 100);
    }

    // Test 9: fail link correctness with shared prefixes
    {
        AhoCorasick ac;
        ac.addPattern("ab");
        ac.addPattern("b");
        ac.build();
        auto hits = ac.search("ab");
        // "ab" ends at 1, "b" ends at 1
        CHECK(hits.size() == 2);
    }

    // Test 10: unicode-ish / special chars
    {
        AhoCorasick ac;
        ac.addPattern("x!y");
        ac.build();
        auto hits = ac.search("ax!ybx!y");
        CHECK(hits.size() == 2);
    }

    std::cout << "Passed: " << g_pass << ", Failed: " << g_fail << std::endl;
    return (g_fail == 0) ? 0 : 1;
}
