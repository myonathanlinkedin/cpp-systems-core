# A Simplified Layout Algebra for GPU Kernels (C++20)

[![Language: C++20](https://img.shields.io/badge/Language-C++20-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/)
[![Standard: Clean Stdlib](https://img.shields.io/badge/Standard-Pure_Stdlib-success?style=flat-square)](#)
[![Systems Domain: Parallel Computing & GPU Systems](https://img.shields.io/badge/Domain-GPU_Systems-8A2BE2?style=flat-square)](#)

A high-performance C++20 layout algebra for multidimensional GPU tensor indexing, coordinate transformations, and zero-copy dimension permutations.

---

## 📐 Algorithmic Overview

This module formalizes multidimensional tensor indexing transformations as an algebraic descriptor `Layout = (Shape, Stride)`:
* **Offset Calculation:** Evaluates $O(1)$ linear memory offset $\text{offset} = \sum_{i=0}^{D-1} \text{indices}[i] \times \text{strides}[i]$.
* **Transposition ($O(1)$ Memory):** Permutes coordinate dimension vectors and stride vectors without copying underlying tensor buffers.
* **Reshape:** Re-associates contiguous coordinate spaces while verifying preservation of total element cardinality.

---

## 🚀 Quickstart & Verification

```bash
g++ -std=c++20 main.cpp core.cpp -I. -O3 -o layout_algebra
./layout_algebra
```

---

## 📑 Associated Mini-Paper

* **Preprint Manuscript:** [A Simplified Layout Algebra for GPU Kernels](https://github.com/myonathanlinkedin/mini-papers/tree/main/papers/2026-10-13_a_simplified_layout_algebra_for_gpu)
* **Preprint PDF:** [`2026-10-13_a_simplified_layout_algebra_for_gpu.pdf`](https://github.com/myonathanlinkedin/mini-papers/blob/main/papers/2026-10-13_a_simplified_layout_algebra_for_gpu/2026-10-13_a_simplified_layout_algebra_for_gpu.pdf)
