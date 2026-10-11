# Hello Algo - 《Hello 算法》：动画图解、一键运行的数据结构与算法教程。支持简中、繁中、English、日本語，提供 Python, Java, C++, C

Self-contained **Hello Algo - 《Hello 算法》：动画图解、一键运行的数据结构与算法教程。支持简中、繁中、English、日本語，提供 Python, Java, C++, C** algorithmic primitive written in idiomatic **C++**. Built from scratch using standard library constructs with zero external dependencies.

### Core Highlights
* **Language & Standard**: Modern `C++` standard library conventions.
* **Architecture Pattern**: Designed for `Algorithmic Engineering` using `Standard Memory Primitives`.
* **Runtime Overhead**: Zero external heap dependencies; designed as a pure in-memory algorithmic component.
* **Concurrency & Safety**: State consistency is verified after mutations through assertion test coverage.

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

*Part of the Polyglot Systems Lab • Maintained by [@myonathanlinkedin](https://github.com/myonathanlinkedin)*