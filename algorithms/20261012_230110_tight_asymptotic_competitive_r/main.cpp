#include <unordered_map>
#include <map>
#include <set>
#include <utility>

#include <iostream>
#include <cassert>
#include "core.cpp"

int main() {
    using namespace ctp;

    // Test 1: Simple line graph, no blocked edges
    {
        Graph g(5);
        g.add_edge(0,1);
        g.add_edge(1,2);
        g.add_edge(2,3);
        g.add_edge(3,4);
        std::unordered_set<std::pair<int,int>, EdgeHash> blocked; // empty
        int offline = offline_optimal_cost(g, 0, 4, blocked);
        int online  = online_travel_cost(g, 0, 4, blocked);
        assert(offline == 4);
        assert(online  == 4);
        double ratio = competitive_ratio(g,0,4,blocked);
        assert(ratio == 1.0);
        std::cout << "Test 1 passed (ratio=" << ratio << ")\n";
    }

    // Test 2: Triangle with one blocked edge forcing detour
    {
        Graph g(3);
        g.add_edge(0,1);
        g.add_edge(1,2);
        g.add_edge(0,2);
        std::unordered_set<std::pair<int,int>, EdgeHash> hidden;
        hidden.insert(make_edge(0,2)); // direct edge blocked
        int offline = offline_optimal_cost(g, 0, 2, hidden);
        int online  = online_travel_cost(g, 0, 2, hidden);
        assert(offline == 2); // must go 0-1-2
        // Online algorithm will try 0-2, discover block (cost 1), then go 0-1-2 (cost 2) = total 3
        assert(online == 3);
        double ratio = competitive_ratio(g,0,2,hidden);
        assert(ratio == 1.5);
        std::cout << "Test 2 passed (ratio=" << ratio << ")\n";
    }

    // Test 3: Square with two blocked edges requiring backtrack
    {
        Graph g(4);
        g.add_edge(0,1);
        g.add_edge(1,2);
        g.add_edge(2,3);
        g.add_edge(0,3);
        g.add_edge(0,2); // diagonal
        std::unordered_set<std::pair<int,int>, EdgeHash> hidden;
        hidden.insert(make_edge(0,3)); // block direct side
        hidden.insert(make_edge(1,2)); // block middle edge
        int offline = offline_optimal_cost(g, 0, 3, hidden);
        // Offline best path: 0-2-3 (2 steps)
        assert(offline == 2);
        int online = online_travel_cost(g, 0, 3, hidden);
        // Possible online walk: try 0-3 (blocked, cost1), try 0-1 (ok, cost2), from 1 try 1-2 (blocked, cost3), backtrack to 0 (cost4), try 0-2 (ok, cost5), then 2-3 (ok, cost6) => total 6
        assert(online == 6);
        double ratio = competitive_ratio(g,0,3,hidden);
        assert(ratio == 3.0);
        std::cout << "Test 3 passed (ratio=" << ratio << ")\n";
    }

    // Test 4: Random small graph, verify online cost >= offline cost
    {
        Graph g(6);
        g.add_edge(0,1);
        g.add_edge(0,2);
        g.add_edge(1,3);
        g.add_edge(2,3);
        g.add_edge(3,4);
        g.add_edge(4,5);
        std::unordered_set<std::pair<int,int>, EdgeHash> hidden;
        hidden.insert(make_edge(1,3)); // block one of the two parallel routes
        int offline = offline_optimal_cost(g, 0, 5, hidden);
        int online  = online_travel_cost(g, 0, 5, hidden);
        assert(offline == 4); // 0-2-3-4-5
        assert(online >= offline);
        double ratio = competitive_ratio(g,0,5,hidden);
        std::cout << "Test 4 passed (offline=" << offline << ", online=" << online << ", ratio=" << ratio << ")\n";
    }

    std::cout << "All tests passed.\n";
    return 0;
}
