#include <vector>
#include <unordered_map>
#include <map>
#include <set>

#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>

using namespace ratalloc;

void test_non_overlapping() {
    // Two virtual registers, never live together, K = 1 should succeed
    std::vector<Instruction> prog = {
        {{0}, {}},          // def r0
        {{}, {0}},          // use r0
        {{1}, {}},          // def r1
        {{}, {1}}           // use r1
    };
    AllocationResult res = allocateRegisters(prog, 1);
    assert(res.spilled.empty());
    assert(res.regMap.size() == 2);
    // Both must map to the same physical register (0)
    assert(res.regMap.at(0) == 0);
    assert(res.regMap.at(1) == 0);
}

void test_overlapping_no_spill() {
    // Three virtual registers, K = 3, all interfere but enough registers
    std::vector<Instruction> prog = {
        {{0}, {}},          // def r0
        {{1}, {0}},         // def r1, use r0
        {{2}, {0,1}},       // def r2, use r0,r1
        {{}, {0,1,2}}       // use all
    };
    AllocationResult res = allocateRegisters(prog, 3);
    assert(res.spilled.empty());
    assert(res.regMap.size() == 3);
    // Verify all registers got distinct colors
    std::unordered_set<int> colors;
    for (auto& kv : res.regMap) colors.insert(kv.second);
    assert(colors.size() == 3);
}

void test_overlapping_with_spill() {
    // Four virtual registers, K = 3, one must spill
    std::vector<Instruction> prog = {
        {{0}, {}},
        {{1}, {0}},
        {{2}, {0,1}},
        {{3}, {0,1,2}},
        {{}, {0,1,2,3}}
    };
    AllocationResult res = allocateRegisters(prog, 3);
    // Exactly one spill expected
    assert(res.spilled.size() == 1);
    // The spilled register must be one of {0,1,2,3}
    for (int v : res.spilled) {
        assert(v >= 0 && v <= 3);
    }
    // All non‑spilled registers receive a color in [0,2]
    for (auto& kv : res.regMap) {
        assert(kv.second >= 0 && kv.second < 3);
    }
}

int main() {
    test_non_overlapping();
    test_overlapping_no_spill();
    test_overlapping_with_spill();
    std::cout << "All Rat's Register Allocator tests passed.\n";
    return 0;
}
