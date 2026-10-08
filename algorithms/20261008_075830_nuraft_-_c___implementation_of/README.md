# Nuraft - C++ implementation of Raft core logic as a replication library

A clean, dependency-free **C++** reference implementation of **Nuraft - C++ implementation of Raft core logic as a replication library**, focused on core algorithmic mechanics, clear memory layout, and test verification.

---

## 🏛️ Architecture & Design Decisions

This module organizes `Nuraft - C++ implementation of Raft core logic as a replication library` into an isolated, self-contained unit:
* **Domain Focus**: `Distributed Consensus & State Machine`
* **Primary Primitives**: `Append-Only State Log & Version Matrix`
* **Memory Strategy**: Buffer boundaries and collection indices are explicitly validated to prevent out-of-bounds access.
* **Correctness Model**: Execution behavior is validated against nominal workflows and boundary edge cases.

### Asymptotic Complexity

| Metric | Bound | Characteristics |
| :--- | :---: | :--- |
| **Best Case Time** | `O(1)` | Optimized fast-path execution |
| **Average / Worst Time** | `O(log N) or O(1)` | Deterministic upper bound for generalized workloads |
| **Space Complexity** | `O(N) state log` | Strict bounds without unconstrained heap growth |

---

## 🧪 Verification Suite

The accompanying `types.hpp` driver executes self-contained verification tests:
1. **Nominal Flow**: Validates baseline correctness under typical real-world inputs.
2. **Boundary Conditions**: Exercises extreme edge cases (empty inputs, singletons, capacity limits).
3. **Invariant Preservation**: Validates internal state consistency throughout mutation lifecycles.

### Running Locally

```bash
g++ -std=c++20 -O3 types.hpp -o runner && ./runner
```

---

*Part of the Polyglot Systems Lab • Maintained by [@myonathanlinkedin](https://github.com/myonathanlinkedin)*