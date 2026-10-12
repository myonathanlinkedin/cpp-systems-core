#include <unordered_map>
#include <map>
#include <set>

#pragma once
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <cassert>

namespace ctp {

// Helper hash for unordered_set of edges (undirected)
struct EdgeHash {
    std::size_t operator()(const std::pair<int,int>& e) const noexcept {
        std::size_t h1 = std::hash<int>{}(e.first);
        std::size_t h2 = std::hash<int>{}(e.second);
        // combine
        return h1 ^ (h2 << 1);
    }
};

inline std::pair<int,int> make_edge(int u, int v) {
    return (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
}

// Simple undirected unit‑weight graph
class Graph {
public:
    Graph(int n = 0) : adj(n) {};

    void add_edge(int u, int v) {
        assert(u >= 0 && v >= 0 && u < (int)adj.size() && v < (int)adj.size());
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    const std::vector<int>& neighbors(int u) const {
        assert(u >= 0 && u < (int)adj.size());
        return adj[u];
    }

    int size() const { return (int)adj.size(); }

private:
    std::vector<std::vector<int>> adj;
};

// Offline optimal cost (shortest path length) given known blocked edges
int offline_optimal_cost(const Graph& g,
                         int s,
                         int t,
                         const std::unordered_set<std::pair<int,int>, EdgeHash>& blocked) {
    std::vector<int> dist(g.size(), -1);
    std::queue<int> q;
    dist[s] = 0;
    q.push(s);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == t) break;
        for (int v : g.neighbors(u)) {
            if (dist[v] != -1) continue;
            if (blocked.find(make_edge(u, v)) != blocked.end()) continue; // edge blocked
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
    return dist[t]; // -1 if unreachable (should not happen in tests)
}

// Online deterministic algorithm (DFS with backtracking)
// Returns total number of traversed edges (each move counts 1)
int online_travel_cost(const Graph& g,
                       int s,
                       int t,
                       const std::unordered_set<std::pair<int,int>, EdgeHash>& hidden_blocked) {
    // Knowledge base of discovered blocked edges
    std::unordered_set<std::pair<int,int>, EdgeHash> known_blocked;
    // Stack for DFS path (stores current node)
    std::stack<int> path;
    path.push(s);
    int total_cost = 0;
    // To avoid infinite loops, keep visited set per node with known blocked context
    std::vector<std::unordered_set<int>> explored(g.size());

    while (!path.empty()) {
        int u = path.top();
        if (u == t) break; // reached target

        // Find an unexplored neighbor that is not known blocked
        bool moved = false;
        for (int v : g.neighbors(u)) {
            if (known_blocked.find(make_edge(u, v)) != known_blocked.end()) continue;
            if (explored[u].find(v) != explored[u].end()) continue; // already tried this direction
            // Mark that we will try this edge from u
            explored[u].insert(v);
            // Simulate discovery: is the edge actually blocked?
            if (hidden_blocked.find(make_edge(u, v)) != hidden_blocked.end()) {
                // Edge is blocked; discover it and stay at u
                known_blocked.insert(make_edge(u, v));
                total_cost += 1; // we attempted to traverse and realized it's blocked (cost of moving to endpoint and back is 1 in unit model)
                continue; // try another neighbor
            } else {
                // Edge is traversable
                path.push(v);
                total_cost += 1; // move to v
                moved = true;
                break;
            }
        }
        if (!moved) {
            // No forward move possible, backtrack
            path.pop();
            if (!path.empty()) {
                total_cost += 1; // move back to previous node
            }
        }
    }
    return total_cost;
}

// Utility to compute competitive ratio (as double)
double competitive_ratio(const Graph& g,
                         int s,
                         int t,
                         const std::unordered_set<std::pair<int,int>, EdgeHash>& hidden_blocked) {
    int offline = offline_optimal_cost(g, s, t, hidden_blocked);
    assert(offline >= 0 && "Target must be reachable in offline instance");
    int online = online_travel_cost(g, s, t, hidden_blocked);
    return static_cast<double>(online) / static_cast<double>(offline);
}

} // namespace ctp
