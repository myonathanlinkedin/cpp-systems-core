#include <vector>
#include <stack>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cassert>

// Tarjan's algorithm for finding strongly connected components in a directed graph.
// Uses an explicit stack to track the current DFS path and a low-link value to detect back edges.

class TarjanSCC {
public:
    explicit TarjanSCC(int n) : n(n), index(n, -1), lowlink(n, -1), onStack(n, false), time(0) {};

    // Add a directed edge from u to v
    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].push_back(v);
        }
    }

    // Run Tarjan's algorithm and return the SCCs as a vector of vectors of node indices.
    std::vector<std::vector<int>> findSCCs() {
        sccs.clear();
        for (int i = 0; i < n; ++i) {
            if (index[i] == -1) {
                strongConnect(i);
            }
        }
        return sccs;
    }

    // Get the number of nodes
    int size() const { return n; }

private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> index;
    std::vector<int> lowlink;
    std::vector<bool> onStack;
    std::stack<int> stack;
    int time;
    std::vector<std::vector<int>> sccs;

    void strongConnect(int v) {
        // Set the depth index for v to the smallest unused index
        index[v] = lowlink[v] = time++;
        stack.push(v);
        onStack[v] = true;

        // Consider each edge from v to w
        for (int w : adj[v]) {
            if (index[w] == -1) {
                // w has not yet been visited; recurse on w
                strongConnect(w);
                lowlink[v] = std::min(lowlink[v], lowlink[w]);
            } else if (onStack[w]) {
                // w is in the stack and thus part of current SCC
                lowlink[v] = std::min(lowlink[v], index[w]);
            }
        }

        // If v is a root node, pop the stack and generate an SCC
        if (lowlink[v] == index[v]) {
            std::vector<int> component;
            int w;
            do {
                w = stack.top();
                stack.pop();
                onStack[w] = false;
                component.push_back(w);
            } while (w != v);
            sccs.push_back(component);
        }
    }
};
