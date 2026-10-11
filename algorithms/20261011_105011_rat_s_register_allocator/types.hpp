#pragma once
#include <vector>
#include <unordered_set>

namespace ratalloc {

struct Instruction {
    // Virtual register identifiers (non‑negative integers)
    std::vector<int> defs;   // registers defined (written) by this instruction
    std::vector<int> uses;   // registers used (read) by this instruction
};

} // namespace ratalloc
