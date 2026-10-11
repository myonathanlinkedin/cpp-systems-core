#include <unordered_map>

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <cassert>
#include <algorithm>
#include <set>
#include <unordered_set>

// Re-declare the class for linkage in this translation unit (in a real project, use a header)
class TarjanSCC {
public:
    explicit TarjanSCC(int n) : n(n), index(n, -1), lowlink(n, -1), onStack(n, false), time(0) {};
    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].push_back(v);
        }
    }
    std::vector<std::vector<int>> findSCCs() {
        sccs.clear();
        for (int i = 0; i < n; ++i) {
            if (index[i] == -1) {
                strongConnect(i);
            }
        }
        return sccs;
    }
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
        index[v] = lowlink[v] = time++;
        stack.push(v);
        onStack[v] = true;
        for (int w : adj[v]) {
            if (index[w] == -1) {
                strongConnect(w);
                lowlink[v] = std::min(lowlink[v], lowlink[w]);
            } else if (onStack[w]) {
                lowlink[v] = std::min(lowlink[v], index[w]);
            }
        }
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

// Helper: Verify that the SCCs form a valid partition of all nodes
bool isValidPartition(const std::vector<std::vector<int>>& sccs, int n) {
    std::unordered_set<int> seen;
    for (const auto& comp : sccs) {
        for (int node : comp) {
            if (node < 0 || node >= n) return false;
            if (seen.count(node)) return false; // Duplicate node
            seen.insert(node);
        }
    }
    return seen.size() == static_cast<size_t>(n);
}

// Helper: Verify that each SCC is truly strongly connected
// (For small graphs, we can check reachability within each component)
bool isStronglyConnected(const std::vector<std::vector<int>>& sccs, const std::vector<std::vector<int>>& adj) {
    for (const auto& comp : sccs) {
        if (comp.size() <= 1) continue;
        int start = comp[0];
        // Check that all nodes in comp are reachable from start
        std::unordered_set<int> reachable;
        std::queue<int> q;
        q.push(start);
        reachable.insert(start);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (!reachable.count(v)) {
                    reachable.insert(v);
                    q.push(v);
                }
            }
        }
        for (int node : comp) {
            if (!reachable.count(node)) return false;
        }
    }
    return true;
}

void testSingleNode() {
    TarjanSCC graph(1);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 1);
    assert(sccs[0].size() == 1);
    assert(sccs[0][0] == 0);
    assert(isValidPartition(sccs, 1));
    std::cout << "PASS: testSingleNode\n";
}

void testTwoNodesNoEdge() {
    TarjanSCC graph(2);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 2);
    assert(isValidPartition(sccs, 2));
    std::cout << "PASS: testTwoNodesNoEdge\n";
}

void testTwoNodesOneEdge() {
    TarjanSCC graph(2);
    graph.addEdge(0, 1);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 2);
    assert(isValidPartition(sccs, 2));
    std::cout << "PASS: testTwoNodesOneEdge\n";
}

void testTwoNodesCycle() {
    TarjanSCC graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 1);
    assert(sccs[0].size() == 2);
    assert(isValidPartition(sccs, 2));
    std::cout << "PASS: testTwoNodesCycle\n";
}

void testThreeNodeCycle() {
    TarjanSCC graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 1);
    assert(sccs[0].size() == 3);
    assert(isValidPartition(sccs, 3));
    std::cout << "PASS: testThreeNodeCycle\n";
}

void testDisconnectedComponents() {
    TarjanSCC graph(6);
    // Component 1: 0 -> 1 -> 2 -> 0
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    // Component 2: 3 -> 4 -> 3
    graph.addEdge(3, 4);
    graph.addEdge(4, 3);
    // Node 5 is isolated
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 3);
    assert(isValidPartition(sccs, 6));
    std::cout << "PASS: testDisconnectedComponents\n";
}

void testSelfLoop() {
    TarjanSCC graph(3);
    graph.addEdge(0, 0);
    graph.addEdge(1, 2);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 3);
    assert(isValidPartition(sccs, 3));
    std::cout << "PASS: testSelfLoop\n";
}

void testCompleteGraph() {
    int n = 5;
    TarjanSCC graph(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) graph.addEdge(i, j);
        }
    }
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 1);
    assert(sccs[0].size() == n);
    assert(isValidPartition(sccs, n));
    std::cout << "PASS: testCompleteGraph\n";
}

void testLinearChain() {
    int n = 5;
    TarjanSCC graph(n);
    for (int i = 0; i < n - 1; ++i) {
        graph.addEdge(i, i + 1);
    }
    auto sccs = graph.findSCCs();
    assert(sccs.size() == n);
    assert(isValidPartition(sccs, n));
    std::cout << "PASS: testLinearChain\n";
}

void testComplexGraph() {
    // Graph:
    // 0 -> 1, 1 -> 2, 2 -> 0 (SCC: {0,1,2})
    // 2 -> 3, 3 -> 4, 4 -> 3 (SCC: {3,4})
    // 4 -> 5 (5 is isolated)
    TarjanSCC graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(4, 3);
    graph.addEdge(4, 5);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 3);
    assert(isValidPartition(sccs, 6));
    std::cout << "PASS: testComplexGraph\n";
}

void testLargeCycle() {
    int n = 100;
    TarjanSCC graph(n);
    for (int i = 0; i < n; ++i) {
        graph.addEdge(i, (i + 1) % n);
    }
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 1);
    assert(sccs[0].size() == n);
    assert(isValidPartition(sccs, n));
    std::cout << "PASS: testLargeCycle\n";
}

void testEmptyGraph() {
    TarjanSCC graph(0);
    auto sccs = graph.findSCCs();
    assert(sccs.size() == 0);
    std::cout << "PASS: testEmptyGraph\n";
}

int main() {
    std::cout << "Running Tarjan SCC unit tests...\n";
    testSingleNode();
    testTwoNodesNoEdge();
    testTwoNodesOneEdge();
    testTwoNodesCycle();
    testThreeNodeCycle();
    testDisconnectedComponents();
    testSelfLoop();
    testCompleteGraph();
    testLinearChain();
    testComplexGraph();
    testLargeCycle();
    testEmptyGraph();
    std::cout << "All tests passed!\n";
    return 0;
}
