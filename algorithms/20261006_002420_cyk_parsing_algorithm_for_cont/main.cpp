#include <string>

#include <iostream>
#include <chrono>
#include <cassert>
#include "core.cpp"

using namespace cyk;

void unit_tests() {
    Grammar G("S");
    // Example grammar from textbook
    G.addNonterminal("S", "A", "B");
    G.addNonterminal("S", "B", "C");
    G.addNonterminal("A", "B", "A");
    G.addTerminal("A", 'a');
    G.addNonterminal("B", "C", "C");
    G.addTerminal("B", 'b');
    G.addNonterminal("C", "A", "B");
    G.addTerminal("C", 'a');
    G.build();

    assert(CYKParser::parse(G, "baaba") && "String baaba should be in the language");
    assert(!CYKParser::parse(G, "ab") && "String ab should NOT be in the language");
    assert(CYKParser::parse(G, "aab") == false);
    assert(CYKParser::parse(G, "a") == false);
    std::cout << "All unit tests passed.\n";
}

void benchmark() {
    Grammar G("S");
    G.addNonterminal("S", "A", "B");
    G.addNonterminal("S", "B", "C");
    G.addNonterminal("A", "B", "A");
    G.addTerminal("A", 'a');
    G.addNonterminal("B", "C", "C");
    G.addTerminal("B", 'b');
    G.addNonterminal("C", "A", "B");
    G.addTerminal("C", 'a');
    G.build();

    std::string long_str(2000, 'a'); // worst‑case for this grammar (mostly rejects)
    auto start = std::chrono::high_resolution_clock::now();
    bool result = CYKParser::parse(G, long_str);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "Parsing 2000‑char string took " << ms.count()
              << " ms. Result: " << std::boolalpha << result << "\n";
}

int main() {
    unit_tests();
    benchmark();
    return 0;
}
