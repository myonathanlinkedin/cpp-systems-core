#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace cyk {

struct Grammar {
    std::string start;
    // productions stored for building lookup tables
    std::vector<std::pair<std::string, std::vector<std::string>>> prods;

    // lookup tables
    std::unordered_map<char, std::unordered_set<std::string>> term_to_lhs;
    std::unordered_map<std::string, std::unordered_set<std::string>> pair_to_lhs;

    explicit Grammar(const std::string& start_symbol) : start(start_symbol) {}

    // A -> a
    void addTerminal(const std::string& lhs, char terminal) {
        prods.emplace_back(lhs, std::vector<std::string>{std::string(1, terminal)});
    }

    // A -> B C
    void addNonterminal(const std::string& lhs,
                        const std::string& rhs1,
                        const std::string& rhs2) {
        prods.emplace_back(lhs, std::vector<std::string>{rhs1, rhs2});
    }

    // Build lookup tables for O(1) access during parsing
    void build() {
        term_to_lhs.clear();
        pair_to_lhs.clear();
        for (const auto& p : prods) {
            const std::string& lhs = p.first;
            const auto& rhs = p.second;
            if (rhs.size() == 1 && rhs[0].size() == 1) {
                char term = rhs[0][0];
                term_to_lhs[term].insert(lhs);
            } else if (rhs.size() == 2) {
                std::string key = rhs[0] + ' ' + rhs[1];
                pair_to_lhs[key].insert(lhs);
            }
        }
    }
};

struct CYKParser {
    static bool parse(const Grammar& G, const std::string& s) {
        size_t n = s.size();
        if (n == 0) return false; // CNF without epsilon productions
        std::vector<std::vector<std::unordered_set<std::string>>> table(
            n, std::vector<std::unordered_set<std::string>>(n));

        // Length 1 substrings
        for (size_t i = 0; i < n; ++i) {
            char a = s[i];
            auto it = G.term_to_lhs.find(a);
            if (it != G.term_to_lhs.end())
                table[i][i] = it->second;
        }

        // Length >=2 substrings
        for (size_t len = 2; len <= n; ++len) {
            for (size_t i = 0; i + len <= n; ++i) {
                size_t j = i + len - 1;
                for (size_t k = i; k < j; ++k) {
                    const auto& left_set = table[i][k];
                    const auto& right_set = table[k + 1][j];
                    if (left_set.empty() || right_set.empty()) continue;
                    for (const auto& B : left_set) {
                        for (const auto& C : right_set) {
                            std::string key = B + ' ' + C;
                            auto pit = G.pair_to_lhs.find(key);
                            if (pit != G.pair_to_lhs.end())
                                table[i][j].insert(pit->second.begin(),
                                                   pit->second.end());
                        }
                    }
                }
            }
        }
        return table[0][n - 1].count(G.start) > 0;
    }
};

} // namespace cyk
