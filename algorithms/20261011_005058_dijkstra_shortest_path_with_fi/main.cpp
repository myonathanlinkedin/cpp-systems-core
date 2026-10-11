#include <vector>
#include <cassert>
#include <numeric>
#include <utility>

#include "types.hpp"
#include "core.hpp"
#include <iostream>

int main() {
    // Test 1: Simple graph
    {
        std::vector<std::vector<std::pair<int,int>>> graph(4);
        graph[0].push_back({1,1});
        graph[0].push_back({2,4});
        graph[1].push_back({0,1});
        graph[1].push_back({2,2});
        graph[1].push_back({3,5});
        graph[2].push_back({0,4});
        graph[2].push_back({1,2});
        graph[2].push_back({3,1});
        graph[3].push_back({1,5});
        graph[3].push_back({2,1});
        auto dist = dijkstra(graph, 0);
        assert(dist.size() == 4);
        assert(dist[0] == 0);
        assert(dist[1] == 1);
        assert(dist[2] == 3);
        assert(dist[3] == 4);
    }
    // Test 2: Unreachable node
    {
        std::vector<std::vector<std::pair<int,int>>> graph(3);
        graph[0].push_back({1,2});
        graph[1].push_back({0,2});
        auto dist = dijkstra(graph, 0);
        assert(dist.size() == 3);
        assert(dist[0] == 0);
        assert(dist[1] == 2);
        assert(dist[2] == std::numeric_limits<int>::max() / 2);
    }
    // Test 3: Cycle
    {
        std::vector<std::vector<std::pair<int,int>>> graph(3);
        graph[0].push_back({1,1});
        graph[1].push_back({2,1});
        graph[2].push_back({0,1});
        auto dist = dijkstra(graph, 0);
        assert(dist.size() == 3);
        assert(dist[0] == 0);
        assert(dist[1] == 1);
        assert(dist[2] == 2);
    }
    // Test 4: Single node
    {
        std::vector<std::vector<std::pair<int,int>>> graph(1);
        auto dist = dijkstra(graph, 0);
        assert(dist.size() == 1);
        assert(dist[0] == 0);
    }
    // Test 5: Empty graph
    {
        std::vector<std::vector<std::pair<int,int>>> graph;
        auto dist = dijkstra(graph, 0);
        assert(dist.empty());
    }
    std::cout << "All tests passed." << std::endl;
    return 0;
}
