#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <functional>
#include <sstream>
#include <set>

namespace cyk {

// Represents a single production rule in the grammar
struct ProductionRule {
    std::string lhs;
    std::vector<std::string> rhs;
    
    ProductionRule() = default;
    ProductionRule(const std::string& l, const std::vector<std::string>& r) : lhs(l), rhs(r) {};
};

// Represents a Context-Free Grammar
struct CFG {
    std::string startSymbol;
    std::vector<ProductionRule> rules;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    
    CFG() = default;
    
    // Build the sets of non-terminals and terminals from rules
    void buildSymbolSets() {
        nonTerminals.clear();
        terminals.clear();
        nonTerminals.insert(startSymbol);
        
        for (const auto& rule : rules) {
            nonTerminals.insert(rule.lhs);
            for (const auto& sym : rule.rhs) {
                // Heuristic: if symbol starts with uppercase, it's a non-terminal
                // Otherwise, it's a terminal
                if (!sym.empty() && std::isupper(sym[0])) {
                    nonTerminals.insert(sym);
                } else {
                    terminals.insert(sym);
                }
            }
        }
    }
    
    // Get all rules for a given non-terminal
    std::vector<const ProductionRule*> getRulesFor(const std::string& nt) const {
        std::vector<const ProductionRule*> result;
        for (const auto& rule : rules) {
            if (rule.lhs == nt) {
                result.push_back(&rule);
            }
        }
        return result;
    }
};

// Represents a parse tree node
struct ParseTreeNode {
    std::string label;
    std::vector<std::shared_ptr<ParseTreeNode>> children;
    
    ParseTreeNode() = default;
    explicit ParseTreeNode(const std::string& l) : label(l) {};
    
    // Get string representation of the tree
    std::string toString(int indent = 0) const {
        std::string result;
        for (int i = 0; i < indent; ++i) result += "  ";
        result += label;
        if (children.empty()) {
            result += "\n";
        } else {
            result += "\n";
            for (const auto& child : children) {
                result += child->toString(indent + 1);
            }
        }
        return result;
    }
};

// Represents a cell in the CYK table
struct CYKCell {
    std::set<std::string> nonTerminals;
    std::shared_ptr<ParseTreeNode> parseTree;
    
    CYKCell() = default;
};

// Represents the complete CYK parsing result
struct CYKResult {
    bool accepted;
    std::vector<std::vector<CYKCell>> table;
    std::shared_ptr<ParseTreeNode> parseTree;
    std::string error;
    
    CYKResult() : accepted(false) {};
};

// Forward declaration
class CYKEngine;

} // namespace cyk
