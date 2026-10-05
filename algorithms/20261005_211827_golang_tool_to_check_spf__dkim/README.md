# Golang tool to check SPF, DKIM, TLSA, and TLS settings for mailservers

A clean, dependency-free **C++** implementation of **Golang tool to check SPF, DKIM, TLSA, and TLS settings for mailservers**, focused on predictable latency, strict memory layout, and deterministic execution.

---

## 🏛️ Architecture & Design Decisions

This module organizes `Golang tool to check SPF, DKIM, TLSA, and TLS settings for mailservers` into an isolated, self-contained unit:
* **Domain Focus**: `Algorithmic Engineering`
* **Primary Primitives**: `Standard Memory Primitives`
* **Memory Strategy**: Memory allocations are kept minimal to avoid allocator contention and preserve CPU cache locality.
* **Correctness Model**: State transitions adhere to strict ordering guarantees with explicit synchronization fences where necessary.

### Asymptotic Complexity

| Metric | Bound | Characteristics |
| :--- | :---: | :--- |
| **Best Case Time** | `$O(1)$` | Optimized fast-path execution |
| **Average / Worst Time** | `$O(N)$` | Deterministic upper bound for generalized workloads |
| **Space Complexity** | `$O(N)$` | Strict bounds without unconstrained heap growth |

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

*Authored & verified by [@myonathanlinkedin](https://github.com/myonathanlinkedin) • Systems Engineering Portfolio*