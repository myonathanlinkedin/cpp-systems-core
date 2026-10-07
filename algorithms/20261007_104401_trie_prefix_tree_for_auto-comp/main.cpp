#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <vector>

int main() {
    lsm::Trie trie;
    trie.insert("apple", 5);
    trie.insert("app", 3);
    trie.insert("application", 2);
    trie.insert("banana", 4);
    trie.insert("band", 1);
    trie.insert("bandana", 2);
    trie.insert("bandage", 1);
    trie.insert("cat", 7);
    trie.insert("cater", 3);
    trie.insert("caterpillar", 1);

    // Test autocomplete for "app"
    auto results_app = trie.autocomplete("app", 10);
    assert(results_app.size() == 3);
    assert(results_app[0].word == "apple");
    assert(results_app[0].frequency == 5);
    assert(results_app[1].word == "app");
    assert(results_app[1].frequency == 3);
    assert(results_app[2].word == "application");
    assert(results_app[2].frequency == 2);

    // Test autocomplete for "ban"
    auto results_ban = trie.autocomplete("ban", 10);
    assert(results_ban.size() == 4);
    assert(results_ban[0].word == "banana");
    assert(results_ban[0].frequency == 4);
    assert(results_ban[1].word == "bandana");
    assert(results_ban[1].frequency == 2);
    assert(results_ban[2].word == "band");
    assert(results_ban[2].frequency == 1);
    assert(results_ban[3].word == "bandage");
    assert(results_ban[3].frequency == 1);

    // Test autocomplete for "cat"
    auto results_cat = trie.autocomplete("cat", 10);
    assert(results_cat.size() == 3);
    assert(results_cat[0].word == "cat");
    assert(results_cat[0].frequency == 7);
    assert(results_cat[1].word == "cater");
    assert(results_cat[1].frequency == 3);
    assert(results_cat[2].word == "caterpillar");
    assert(results_cat[2].frequency == 1);

    // Edge case: prefix not present
    auto results_none = trie.autocomplete("xyz", 5);
    assert(results_none.empty());

    // Edge case: empty prefix returns all words sorted by frequency
    auto results_all = trie.autocomplete("", 20);
    assert(results_all.size() == 10);
    assert(results_all[0].word == "cat");
    assert(results_all[0].frequency == 7);
    assert(results_all[1].word == "apple");
    assert(results_all[1].frequency == 5);
    assert(results_all[2].word == "banana");
    assert(results_all[2].frequency == 4);
    assert(results_all[3].word == "app");
    assert(results_all[3].frequency == 3);
    assert(results_all[4].word == "cater");
    assert(results_all[4].frequency == 3);
    assert(results_all[5].word == "application");
    assert(results_all[5].frequency == 2);
    assert(results_all[6].word == "bandana");
    assert(results_all[6].frequency == 2);
    assert(results_all[7].word == "band");
    assert(results_all[7].frequency == 1);
    assert(results_all[8].word == "bandage");
    assert(results_all[8].frequency == 1);
    assert(results_all[9].word == "caterpillar");
    assert(results_all[9].frequency == 1);

    std::cout << "All tests passed.\n";
    return 0;
}
