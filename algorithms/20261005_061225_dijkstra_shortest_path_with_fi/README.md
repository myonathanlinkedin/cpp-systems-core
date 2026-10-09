# Dijkstra Shortest Path with Fibonacci Heap Priority Queue

Self-contained **Dijkstra Shortest Path with Fibonacci Heap Priority Queue** algorithmic primitive written in idiomatic **C++**. Built from scratch using standard library constructs with zero external dependencies.

---

## 🏛️ Architecture & Design Decisions

This module organizes `Dijkstra Shortest Path with Fibonacci Heap Priority Queue` into an isolated, self-contained unit:
* **Domain Focus**: `Graph Topology & Traversal`
* **Primary Primitives**: `Adjacency List & Priority Heap`
* **Memory Strategy**: Buffer boundaries and collection indices are explicitly validated to prevent out-of-bounds access.
* **Correctness Model**: State transitions follow clear ordering guarantees with explicit validation at each phase.

### Asymptotic Complexity

| Metric | Bound | Characteristics |
| :--- | :---: | :--- |
| **Best Case Time** | `O(V + E)` | Optimized fast-path execution |
| **Average / Worst Time** | `O((V + E) log V)` | Deterministic upper bound for generalized workloads |
| **Space Complexity** | `O(V + E)` | Strict bounds without unconstrained heap growth |

---

## 🧪 Verification Suite

The accompanying `main.cpp` driver executes self-contained verification tests:
1. **Nominal Flow**: Validates baseline correctness under typical real-world inputs.
2. **Boundary Conditions**: Exercises extreme edge cases (empty inputs, singletons, capacity limits).
3. **Invariant Preservation**: Validates internal state consistency throughout mutation lifecycles.

### Running Locally

```bash
g++ -std=c++20 -O3 main.cpp -o runner && ./runner
```

---

*Part of the Polyglot Systems Lab • Maintained by [@myonathanlinkedin](https://github.com/myonathanlinkedin)*
