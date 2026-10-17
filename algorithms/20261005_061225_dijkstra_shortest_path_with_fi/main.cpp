#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <cassert>
#include <cmath>

using namespace std;

template <typename K, typename V>
class FibonacciHeap {
public:
    struct Node {
        K key;
        V value;
        int degree = 0;
        bool mark = false;
        Node *parent = nullptr, *child = nullptr;
        Node *left, *right;
        Node(const K& k, const V& v) : key(k), value(v), left(this), right(this) {}
    };

    FibonacciHeap() : minNode(nullptr), n(0) {}
    ~FibonacciHeap() { clear(minNode); }

    Node* insert(const K& key, const V& value) {
        Node* x = new Node(key, value);
        if (!minNode) minNode = x;
        else {
            mergeLists(minNode, x);
            if (key < minNode->key) minNode = x;
        }
        ++n;
        return x;
    }

    bool empty() const { return minNode == nullptr; }

    const Node* top() const { return minNode; }

    Node* extract_min() {
        Node* z = minNode;
        if (z) {
            if (z->child) {
                Node* c = z->child;
                do {
                    Node* nxt = c->right;
                    mergeLists(minNode, c);
                    c->parent = nullptr;
                    c = nxt;
                } while (c != z->child);
                z->child = nullptr;
            }
            removeNode(z);
            if (z == z->right) minNode = nullptr;
            else {
                minNode = z->right;
                consolidate();
            }
            --n;
        }
        return z;
    }

    void decrease_key(Node* x, const K& newKey) {
        assert(newKey <= x->key);
        x->key = newKey;
        Node* y = x->parent;
        if (y && x->key < y->key) {
            cut(x, y);
            cascading_cut(y);
        }
        if (x->key < minNode->key) minNode = x;
    }

private:
    Node* minNode;
    size_t n;

    static void mergeLists(Node* a, Node* b) {
        if (!a || !b) return;
        Node* an = a->right;
        Node* bp = b->left;
        a->right = b;
        b->left = a;
        an->left = bp;
        bp->right = an;
    }

    static void removeNode(Node* x) {
        x->left->right = x->right;
        x->right->left = x->left;
    }

    void consolidate() {
        size_t D = static_cast<size_t>(log2(n)) + 1;
        vector<Node*> A(D, nullptr);
        vector<Node*> roots;
        Node* x = minNode;
        if (x) {
            do { roots.push_back(x); x = x->right; } while (x != minNode);
        }
        for (Node* w : roots) {
            x = w;
            int d = x->degree;
            while (A[d]) {
                Node* y = A[d];
                if (x->key > y->key) swap(x, y);
                link(y, x);
                A[d] = nullptr;
                ++d;
            }
            A[d] = x;
        }
        minNode = nullptr;
        for (Node* y : A) if (y) {
            if (!minNode) { y->left = y->right = y; minNode = y; }
            else {
                mergeLists(minNode, y);
                if (y->key < minNode->key) minNode = y;
            }
        }
    }

    void link(Node* y, Node* x) {
        removeNode(y);
        y->parent = x;
        y->mark = false;
        if (!x->child) { x->child = y; y->left = y->right = y; }
        else {
            mergeLists(x->child, y);
        }
        ++x->degree;
    }

    void cut(Node* x, Node* y) {
        if (y->child == x) {
            if (x->right != x) y->child = x->right;
            else y->child = nullptr;
        }
        removeNode(x);
        --y->degree;
        mergeLists(minNode, x);
        x->parent = nullptr;
        x->mark = false;
    }

    void cascading_cut(Node* y) {
        Node* z = y->parent;
        if (z) {
            if (!y->mark) y->mark = true;
            else {
                cut(y, z);
                cascading_cut(z);
            }
        }
    }

    static void clear(Node* start) {
        if (!start) return;
        Node* cur = start;
        do {
            Node* nxt = cur->right;
            if (cur->child) clear(cur->child);
            delete cur;
            cur = nxt;
        } while (cur != start);
    }
};

vector<double> dijkstra_fib(const vector<vector<pair<int,double>>>& adj, int src) {
    const double INF = numeric_limits<double>::infinity();
    int n = adj.size();
    vector<double> dist(n, INF);
    vector<FibonacciHeap<double,int>::Node*> nodes(n, nullptr);
    FibonacciHeap<double,int> pq;
    dist[src] = 0.0;
    for (int i=0;i<n;++i) nodes[i] = pq.insert(dist[i], i);
    while (!pq.empty()) {
        auto* minNode = pq.extract_min();
        int u = minNode->value;
        double du = minNode->key;
        delete minNode;
        if (du != dist[u]) continue;
        for (auto [v,w]: adj[u]) {
            double nd = du + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                pq.decrease_key(nodes[v], nd);
            }
        }
    }
    return dist;
}

int main() {
    // Simple test graph
    // 0 --1--> 1 --2--> 2
    // 0 --4--> 2
    vector<vector<pair<int,double>>> g(3);
    g[0].push_back({1,1.0});
    g[1].push_back({2,2.0});
    g[0].push_back({2,4.0});
    auto d = dijkstra_fib(g,0);
    assert(fabs(d[0]-0.0) < 1e-9);
    assert(fabs(d[1]-1.0) < 1e-9);
    assert(fabs(d[2]-3.0) < 1e-9);
    // Larger test
    int N = 5;
    vector<vector<pair<int,double>>> h(N);
    h[0] = {{1,10},{2,3}};
    h[1] = {{2,1},{3,2}};
    h[2] = {{1,4},{3,8},{4,2}};
    h[3] = {{4,7}};
    h[4] = {{3,9}};
    auto d2 = dijkstra_fib(h,0);
    vector<double> exp = {0,7,3,9,5};
    for(int i=0;i<N;++i) assert(fabs(d2[i]-exp[i])<1e-9);
    cout << "All tests passed.\n";
    return 0;
}
