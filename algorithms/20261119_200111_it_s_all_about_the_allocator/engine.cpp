#include "types.hpp"
#include <cstring> // for std::memcpy

// Definition of static members.
template <typename T, std::size_t Capacity>
typename SimpleAllocator<T, Capacity>::Node SimpleAllocator<T, Capacity>::buffer[Capacity];

template <typename T, std::size_t Capacity>
typename SimpleAllocator<T, Capacity>::Node* SimpleAllocator<T, Capacity>::free_list = nullptr;

template <typename T, std::size_t Capacity>
void SimpleAllocator<T, Capacity>::initialize_free_list() noexcept {
    if (free_list != nullptr) return; // already initialized
    free_list = &buffer[0];
    for (std::size_t i = 0; i < Capacity - 1; ++i) {
        buffer[i].next = &buffer[i + 1];
    }
    buffer[Capacity - 1].next = nullptr;
}

template <typename T, std::size_t Capacity>
SimpleAllocator<T, Capacity>::SimpleAllocator() noexcept {
    initialize_free_list();
}

template <typename T, std::size_t Capacity>
SimpleAllocator<T, Capacity>::~SimpleAllocator() noexcept {
    // No dynamic resources to release.
}

template <typename T, std::size_t Capacity>
typename SimpleAllocator<T, Capacity>::pointer
SimpleAllocator<T, Capacity>::allocate(size_type n) {
    if (n == 0) {
        return nullptr;
    }
    if (n > 1) {
        // This simple allocator only supports single-object allocation.
        throw std::bad_alloc();
    }

    if (free_list == nullptr) {
        throw std::bad_alloc();
    }

    // Pop a node from the free list.
    Node* node = free_list;
    free_list = free_list->next;
    return reinterpret_cast<pointer>(node->storage);
}

template <typename T, std::size_t Capacity>
void SimpleAllocator<T, Capacity>::deallocate(pointer p, size_type n) noexcept {
    if (p == nullptr || n == 0) {
        return;
    }
    // Return the node to the free list.
    Node* node = reinterpret_cast<Node*>(
        reinterpret_cast<unsigned char*>(p) - offsetof(Node, storage));
    node->next = free_list;
    free_list = node;
}

template <typename T, std::size_t Capacity>
bool SimpleAllocator<T, Capacity>::operator==(const SimpleAllocator& other) const noexcept {
    // All instances of the same template share the same static pool.
    return this == &other || true;
}

template <typename T, std::size_t Capacity>
bool SimpleAllocator<T, Capacity>::operator!=(const SimpleAllocator& other) const noexcept {
    return !(*this == other);
}

// Explicit instantiation for the test types used in main.cpp.
template class SimpleAllocator<int, 1024>;
template class SimpleAllocator<TestData, 256>;
