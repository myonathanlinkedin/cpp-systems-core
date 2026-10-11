#include <unordered_map>
#include <map>
#include <set>
#include <utility>

#include "core.hpp"
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <cassert>

namespace ratalloc {

using AdjList = std::unordered_map<int, std::unordered_set<int>>;

// Compute live‑out sets for each program point (backward data‑flow)
static std::vector<std::unordered_set<int>> computeLiveOut(const std::vector<Instruction>& instrs) {
    size_t n = instrs.size();
    std::vector<std::unordered_set<int>> liveOut(n + 1); // liveOut[i] = live after instr i (i == n => after last)
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            // liveOut[i] = (liveOut[i+1] - defs) ∪ uses
            std::unordered_set<int> newSet = liveOut[i + 1];
            for (int d : instrs[i].defs) {
                newSet.erase(d);
            }
            newSet.insert(instrs[i].uses.begin(), instrs[i].uses.end());
            if (newSet != liveOut[i]) {
                liveOut[i] = std::move(newSet);
                changed = true;
            }
        }
    }
    return liveOut;
}

// Build interference graph from live‑out information
static AdjList buildInterference(const std::vector<Instruction>& instrs,
                                 const std::vector<std::unordered_set<int>>& liveOut) {
    AdjList graph;
    for (size_t i = 0; i < instrs.size(); ++i) {
        // Ensure all defs appear as nodes
        for (int d : instrs[i].defs) {
            graph.emplace(d, std::unordered_set<int>{});
        }
        // Interfere each def with every live register after the instruction
        for (int d : instrs[i].defs) {
            for (int live : liveOut[i + 1]) {
                if (live == d) continue;
                graph[d].insert(live);
                graph[live].insert(d);
            }
        }
    }
    return graph;
}

// Simplify phase: produce stack of nodes for coloring
static std::vector<std::pair<int, bool>> simplify(const AdjList& origGraph, int K) {
    AdjList graph = origGraph; // mutable copy
    std::vector<std::pair<int, bool>> stack; // (node, maySpill)
    while (!graph.empty()) {
        // Find a node with degree < K
        int candidate = -1;
        for (const auto& kv : graph) {
            if (static_cast<int>(kv.second.size()) < K) {
                candidate = kv.first;
                break;
            }
        }
        bool willSpill = false;
        if (candidate == -1) {
            // No low‑degree node → pick a spill candidate (highest degree)
            int maxDeg = -1;
            for (const auto& kv : graph) {
                int deg = static_cast<int>(kv.second.size());
                if (deg > maxDeg) {
                    maxDeg = deg;
                    candidate = kv.first;
                }
            }
            willSpill = true;
        }
        // Push onto stack and remove from graph
        stack.emplace_back(candidate, willSpill);
        // Erase edges
        for (int neighbor : graph[candidate]) {
            graph[neighbor].erase(candidate);
        }
        graph.erase(candidate);
    }
    return stack;
}

// Select phase: assign colors (registers) or mark spill
static AllocationResult select(const std::vector<std::pair<int, bool>>& stack, const AdjList& origGraph, int K) {
    AllocationResult result;
    // Process in reverse order of simplification
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        int node = it->first;
        const auto& neighbors = origGraph.at(node);
        // Determine which colors are already used by colored neighbors
        bool used[64] = {false}; // K is expected to be small; guard against overflow
        for (int nb : neighbors) {
            auto itMap = result.regMap.find(nb);
            if (itMap != result.regMap.end()) {
                int col = itMap->second;
                if (col >= 0 && col < 64) used[col] = true;
            }
        }
        // Find first free color
        int chosen = -1;
        for (int c = 0; c < K; ++c) {
            if (!used[c]) {
                chosen = c;
                break;
            }
        }
        if (chosen != -1) {
            result.regMap[node] = chosen;
        } else {
            // No color available → spill
            result.spilled.insert(node);
        }
    }
    return result;
}

AllocationResult allocateRegisters(const std::vector<Instruction>& instrs, int K) {
    assert(K > 0 && "Number of physical registers must be positive");
    // 1. Liveness analysis
    auto liveOut = computeLiveOut(instrs);
    // 2. Interference graph construction
    AdjList graph = buildInterference(instrs, liveOut);
    // 3. Simplify
    auto stack = simplify(graph, K);
    // 4. Select (coloring)
    AllocationResult res = select(stack, graph, K);
    return res;
}

} // namespace ratalloc
