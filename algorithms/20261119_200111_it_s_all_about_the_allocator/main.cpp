#include <cmath>

#include "types.hpp"
#include "engine.cpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

int main() {
    // Basic allocation/deallocation test for int.
    {
        SimpleAllocator<int, 1024> alloc;
        int* p1 = alloc.allocate(1);
        assert(p1 != nullptr);
        *p1 = 42;
        assert(*p1 == 42);
        alloc.deallocate(p1, 1);
    }

    // Exhaustive allocation until pool is empty.
    {
        SimpleAllocator<int, 4> alloc;
        int* a[4];
        for (int i = 0; i < 4; ++i) {
            a[i] = alloc.allocate(1);
            assert(a[i] != nullptr);
            *a[i] = i * 10;
        }
        // Next allocation must fail.
        bool caught = false;
        try {
            alloc.allocate(1);
        } catch (const std::bad_alloc&) {
            caught = true;
        }
        assert(caught && "Expected std::bad_alloc when pool exhausted");

        // Deallocate in reverse order and reallocate.
        for (int i = 3; i >= 0; --i) {
            alloc.deallocate(a[i], 1);
        }
        for (int i = 0; i < 4; ++i) {
            int* p = alloc.allocate(1);
            assert(p != nullptr);
            *p = i + 100;
            alloc.deallocate(p, 1);
        }
    }

    // Test with user-defined struct.
    {
        SimpleAllocator<TestData, 2> alloc;
        TestData* td1 = alloc.allocate(1);
        new (td1) TestData(7, 3.14); // placement new
        assert(td1->id == 7 && std::abs(td1->value - 3.14) < 1e-9);
        td1->~TestData(); // explicit destructor
        alloc.deallocate(td1, 1);

        // Allocate second object.
        TestData* td2 = alloc.allocate(1);
        new (td2) TestData(8, 2.71);
        assert(td2->id == 8 && std::abs(td2->value - 2.71) < 1e-9);
        td2->~TestData();
        alloc.deallocate(td2, 1);
    }

    // Edge case: allocate zero elements should return nullptr and not throw.
    {
        SimpleAllocator<int, 10> alloc;
        int* p = alloc.allocate(0);
        assert(p == nullptr);
        alloc.deallocate(p, 0); // should be a no-op
    }

    // Verify that allocator can be used with std::vector via rebind.
    {
        SimpleAllocator<int, 16> alloc;
        using VecAlloc = SimpleAllocator<int, 16>::rebind<int>::other;
        std::vector<int, VecAlloc> vec(alloc);
        vec.reserve(8);
        for (int i = 0; i < 8; ++i) {
            vec.push_back(i * i);
        }
        for (int i = 0; i < 8; ++i) {
            assert(vec[i] == i * i);
        }
    }

    std::cout << "All allocator unit tests passed.\n";
    return 0;
}
