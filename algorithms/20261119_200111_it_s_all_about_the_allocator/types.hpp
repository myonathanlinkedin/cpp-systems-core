#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

// Simple fixed-size pool allocator template.
// The allocator manages a pre-allocated buffer capable of holding
// up to Capacity objects of type T. It satisfies the minimal
// requirements of a C++ allocator.
template <typename T, std::size_t Capacity>
class SimpleAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // Rebind support for allocator-aware containers.
    template <typename U>
    struct rebind {
        using other = SimpleAllocator<U, Capacity>;
    };

    SimpleAllocator() noexcept;
    ~SimpleAllocator() noexcept;

    // Copy constructor (required by allocator model).
    template <typename U>
    SimpleAllocator(const SimpleAllocator<U, Capacity>&) noexcept {}

    // Allocate memory for n objects of type T.
    // Throws std::bad_alloc if the request cannot be satisfied.
    pointer allocate(size_type n);

    // Deallocate memory previously allocated by allocate().
    void deallocate(pointer p, size_type n) noexcept;

    // Equality operators (allocators are interchangeable if they manage the same pool).
    bool operator==(const SimpleAllocator& other) const noexcept;
    bool operator!=(const SimpleAllocator& other) const noexcept;

private:
    // Internal node structure for free list management.
    struct Node {
        alignas(T) unsigned char storage[sizeof(T)];
        Node* next;
    };

    // Static buffer of Nodes.
    static Node buffer[Capacity];
    static Node* free_list;

    // Helper to initialize the free list on first use.
    static void initialize_free_list() noexcept;
};

// Helper struct used in unit tests.
struct TestData {
    int id;
    double value;
    TestData() : id(0), value(0.0) {};
    TestData(int i, double v) : id(i), value(v) {}
};
