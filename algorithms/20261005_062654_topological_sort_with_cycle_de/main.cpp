#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <algorithm>

class DirectedGraph {
public:
    explicit DirectedGraph(size_t n) : adj(n), indeg(n, 0) {}

    void addEdge(size_t u, size_t v) {
        assert(u < adj.size() && v < adj.size());
        adj[u].push_back(v);
        ++indeg[v];
    }

    // Returns {true, order} if DAG, else {false, empty}
    std::pair<bool, std::vector<size_t>> topologicalSort() const {
        size_t n = adj.size();
        std::vector<size_t> indegCopy = indeg;
        std::queue<size_t> q;
        for (size_t i = 0; i < n; ++i)
            if (indegCopy[i] == 0) q.push(i);

        std::vector<size_t> order;
        order.reserve(n);
        while (!q.empty()) {
            size_t u = q.front(); q.pop();
            order.push_back(u);
            for (size_t v : adj[u]) {
                if (--indegCopy[v] == 0) q.push(v);
            }
        }
        return {order.size() == n, order};
    }

private:
    std::vector<std::vector<size_t>> adj;
    std::vector<size_t> indeg;
};

void testSimpleDAG() {
    DirectedGraph g(6);
    g.addEdge(5, 2);
    g.addEdge(5, 0);
    g.addEdge(4, 0);
    g.addEdge(4, 1);
    g.addEdge(2, 3);
    g.addEdge(3, 1);
    auto [ok, order] = g.topologicalSort();
    assert(ok);
    // Verify ordering constraints
    auto pos = [&](size_t v){ return std::find(order.begin(), order.end(), v) - order.begin(); };
    assert(pos(5) < pos(2));
    assert(pos(5) < pos(0));
    assert(pos(4) < pos(0));
    assert(pos(4) < pos(1));
    assert(pos(2) < pos(3));
    assert(pos(3) < pos(1));
}

void testCycleDetection() {
    DirectedGraph g(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0); // cycle
    auto [ok, order] = g.topologicalSort();
    assert(!ok);
    assert(order.empty());
}

void testMultipleComponents() {
    DirectedGraph g(8);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(3, 4);
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    auto [ok, order] = g.topologicalSort();
    assert(ok);
    // Ensure each component respects its internal order
    auto pos = [&](size_t v){ return std::find(order.begin(), order.end(), v) - order.begin(); };
    assert(pos(0) < pos(1) && pos(1) < pos(2));
    assert(pos(3) < pos(4));
    assert(pos(5) < pos(6) && pos(6) < pos(7));
}

int main() {
    testSimpleDAG();
    testCycleDetection();
    testMultipleComponents();
    std::cout << "All tests passed.\n";
    return 0;
}
