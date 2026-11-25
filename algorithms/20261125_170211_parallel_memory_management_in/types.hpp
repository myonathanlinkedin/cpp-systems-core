#pragma once

#include <cstddef>
#include <atomic>
#include <new>
#include <type_traits>

namespace mem {

template <typename T>
class MemoryPool {
public:
    MemoryPool() noexcept;
    ~MemoryPool() noexcept;

    // Non‑copyable, non‑movable
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    T* allocate();
    void deallocate(T* ptr) noexcept;

private:
    struct Node {
        Node* next;
    };
    std::atomic<Node*> free_list_;

    static Node* ptr_to_node(T* p) noexcept {
        return reinterpret_cast<Node*>(p);
    }
    static T* node_to_ptr(Node* n) noexcept {
        return reinterpret_cast<T*>(n);
    }
};

struct TestData {
    int id;
    double value;
    TestData() : id(0), value(0.0) {};
    TestData(int i, double v) : id(i), value(v) {}
};

} // namespace mem
