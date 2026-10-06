# Tarjan Strongly Connected Components Search in Directed Graphs (C++)

> Self-contained **Tarjan Strongly Connected Components Search in Directed Graphs** algorithmic primitive written in idiomatic **C++**. Built from scratch using standard library constructs with zero external dependencies.

## Overview & Mechanics

The implementation focuses on the core mathematical properties of **Tarjan Strongly Connected Components Search in Directed Graphs**:
* **Data Organization**: Built upon `Adjacency List & Priority Heap` to ensure predictable traversal and storage overhead.
* **Safety Invariants**: Buffer boundaries and collection indices are explicitly validated to prevent out-of-bounds access.
* **Execution Guarantees**: Execution behavior is validated against nominal workflows and boundary edge cases.

## Complexity Profile

* **Time Complexity**:
  * Fast Path (Best): `$O(V + E)$`
  * Generalized (Avg / Worst): `$O((V + E) \log V)$`
* **Space Footprint**: `$O(V + E)$` resident heap / stack overhead.

## Verification & Test Scenarios

The test suite in `main.cpp` validates:
* Standard operational paths against expected outcomes.
* Extreme values and edge inputs to ensure robust failure handling.
* State stability across sequential and repeated operations.

```bash
# Execute local verification runner
g++ -std=c++20 -O3 main.cpp -o runner && ./runner
```

---

*Reference implementation verified by [@myonathanlinkedin](https://github.com/myonathanlinkedin)*