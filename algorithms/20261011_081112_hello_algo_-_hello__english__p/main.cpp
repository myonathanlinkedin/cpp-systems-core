#include <cassert>
#include <iostream>
#include <vector>
#include "core.cpp"   // Include implementation; single translation unit.

int main() {
    using namespace lsm;

    // Test greet()
    assert(greet() == "Hello, Algo!");

    // Test Fibonacci (first 10 numbers)
    const uint64_t fib_expected[] = {0,1,1,2,3,5,8,13,21,34,55};
    for (uint64_t i = 0; i <= 10; ++i) {
        assert(fibonacci(i) == fib_expected[i]);
    }

    // Test stable_sort_ints()
    {
        std::vector<int> v = {5, 3, 5, 2, 1, 3};
        stable_sort_ints(v);
        const std::vector<int> expected = {1,2,3,3,5,5};
        assert(v == expected);
    }

    // Test reverse_vector()
    {
        std::vector<char> v = {'a','b','c','d'};
        reverse_vector(v);
        const std::vector<char> expected = {'d','c','b','a'};
        assert(v == expected);
    }

    // Test max_subarray_sum()
    {
        std::vector<int> v = {-2,1,-3,4,-1,2,1,-5,4};
        assert(max_subarray_sum(v) == 6); // Subarray [4,-1,2,1]
        std::vector<int> all_negative = {-8,-3,-6};
        assert(max_subarray_sum(all_negative) == -3);
        std::vector<int> empty;
        assert(max_subarray_sum(empty) == 0);
    }

    // Test binary_search()
    {
        std::vector<int> sorted = {1,3,5,7,9,11};
        assert(binary_search(sorted, 7) == 3);
        assert(binary_search(sorted, 2) == -1);
        assert(binary_search(sorted, 11) == 5);
        assert(binary_search(sorted, 1) == 0);
    }

    // All tests passed.
    std::cout << "All unit tests passed successfully.\n";
    return 0;
}
