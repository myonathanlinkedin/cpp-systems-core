#include <vector>
#include <map>
#include <set>

#pragma once
#include "types.hpp"
#include <unordered_map>
#include <unordered_set>

namespace ratalloc {

// Result of register allocation
struct AllocationResult {
    // Mapping from virtual register -> physical register (0 .. K-1)
    std::unordered_map<int, int> regMap;
    // Set of virtual registers that could not be allocated (spilled)
    std::unordered_set<int> spilled;
};

// Perform graph‑coloring register allocation on a single basic block.
// Parameters:
//   instrs – sequence of instructions (linear order, no control flow)
//   K      – number of available physical registers
// Returns:
//   AllocationResult containing the mapping and spill set.
AllocationResult allocateRegisters(const std::vector<Instruction>& instrs, int K);

} // namespace ratalloc
