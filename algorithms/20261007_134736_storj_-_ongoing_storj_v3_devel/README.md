# Storj - Ongoing Storj v3 development. Decentralized cloud object storage that is

An in-memory reference implementation of **Storj - Ongoing Storj v3 development. Decentralized cloud object storage that is** in **C++**, adhering to standard library idioms, clean data structures, and assertion test suites.

### Core Highlights
* **Language & Standard**: Modern `C++` standard library conventions.
* **Architecture Pattern**: Designed for `Algorithmic Engineering` using `Standard Memory Primitives`.
* **Runtime Overhead**: Zero external heap dependencies; designed as a pure in-memory algorithmic component.
* **Concurrency & Safety**: Encapsulates state within isolated data structures, keeping logic self-contained.

---

### Complexity Analysis

| Dimension | Bound |
| :--- | :--- |
| **Time (Best Case)** | `O(1)` |
| **Time (Worst Case)** | `O(N log N)` |
| **Auxiliary Space** | `O(N)` |

---

### Test Suite Execution

Self-contained verification drivers are embedded directly in `main.cpp` to validate happy paths, boundary inputs, and invariant preservation.

```bash
g++ -std=c++20 -O3 main.cpp -o runner && ./runner
```

---

<sub>Standard C++ reference implementation • Maintained by [@myonathanlinkedin](https://github.com/myonathanlinkedin)</sub>