#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>

int main() {
    using namespace lsm;
    // Test 1: Empty matrix
    Matrix empty;
    Matrix trans_empty;
    transpose(empty, trans_empty);
    assert(trans_empty.empty());
    std::cout << "Test 1 passed: Empty matrix.\n";

    // Test 2: 1x1 matrix
    Matrix one = {{42}};
    Matrix trans_one;
    transpose(one, trans_one);
    assert(trans_one.size() == 1 && trans_one[0].size() == 1 && trans_one[0][0] == 42);
    std::cout << "Test 2 passed: 1x1 matrix.\n";

    // Test 3: Square matrix 8x8
    Matrix sq = generate_random_matrix(8, 8);
    Matrix trans_sq;
    transpose(sq, trans_sq);
    assert(matrices_equal(trans_sq, naive_transpose(sq)));
    std::cout << "Test 3 passed: 8x8 square matrix.\n";

    // Test 4: Rectangular matrix 5x12
    Matrix rect = generate_random_matrix(5, 12);
    Matrix trans_rect;
    transpose(rect, trans_rect);
    assert(matrices_equal(trans_rect, naive_transpose(rect)));
    std::cout << "Test 4 passed: 5x12 rectangular matrix.\n";

    // Test 5: Large matrix 256x256
    Matrix large = generate_random_matrix(256, 256);
    Matrix trans_large;
    transpose(large, trans_large);
    assert(matrices_equal(trans_large, naive_transpose(large)));
    std::cout << "Test 5 passed: 256x256 large matrix.\n";

    // Test 6: Involution property
    Matrix inv = generate_random_matrix(13, 7);
    Matrix trans_inv;
    transpose(inv, trans_inv);
    Matrix back;
    transpose(trans_inv, back);
    assert(matrices_equal(back, inv));
    std::cout << "Test 6 passed: Involution property.\n";

    std::cout << "All tests passed successfully.\n";
    return 0;
}
