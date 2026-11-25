#include "types.hpp"
#include <cstdlib>
#include <cassert>

namespace mem {

template <typename T>
MemoryPool<T>::MemoryPool() noexcept : free_list_(nullptr) {}

template <typename T>
MemoryPool<T>::~MemoryPool() noexcept {
    // Release all nodes retained in the free list
    Node* node = free_list_.load(std::memory_order_relaxed);
    while (node) {
        Node* next = node->next;
        ::operator delete(node);
        node = next;
    }
}

template <typename T>
T* MemoryPool<T>::allocate() {
    Node* head = free_list_.load(std::memory_order_acquire);
    while (head) {
        Node* next = head->next;
        if (free_list_.compare_exchange_weak(head, next,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            T* obj = node_to_ptr(head);
            new (obj) T();               // placement‑new default construction
            return obj;
        }
        // head refreshed, retry
    }
    // Free list empty – allocate fresh storage
    void* raw = ::operator new(sizeof(T));
    T* obj = static_cast<T*>(raw);
    new (obj) T();
    return obj;
}

template <typename T>
void MemoryPool<T>::deallocate(T* ptr) noexcept {
    if (!ptr) return;
    ptr->~T();                           // explicit destructor call
    Node* node = ptr_to_node(ptr);
    Node* head = free_list_.load(std::memory_order_relaxed);
    do {
        node->next = head;
    } while (!free_list_.compare_exchange_weak(head, node,
            std::memory_order_release, std::memory_order_relaxed));
}

// Explicit instantiation for the only type used in tests
template class MemoryPool<TestData>;

} // namespace mem
