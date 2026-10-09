# Topological Sort with Cycle Detection in Directed Graphs in C++

An in-memory reference implementation of **Topological Sort with Cycle Detection in Directed Graphs** in **C++**, adhering to standard library idioms, clean data structures, and assertion test suites.

## Implementation Details

* **Category**: `Graph Topology & Traversal`
* **Data Structure Foundation**: `Adjacency List & Priority Heap`
* **Allocation Pattern**: Zero external heap dependencies; designed as a pure in-memory algorithmic component.
* **Invariant Integrity**: Encapsulates state within isolated data structures, keeping logic self-contained.

## Performance Characteristics

* **Time**: `O((V + E) log V)` average, with `O(V + E)` best-case response under ideal conditions.
* **Space**: `O(V + E)` memory usage.

## Test Harness

To compile and execute the test assertions for this module:

```bash
g++ -std=c++20 -O3 main.cpp -o runner && ./runner
```

---

*Source code released under the MIT License • [@myonathanlinkedin](https://github.com/myonathanlinkedin)*
