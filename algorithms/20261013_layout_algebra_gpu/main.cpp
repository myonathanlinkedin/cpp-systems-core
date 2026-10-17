#include "core.hpp"
#include <iostream>
#include <cassert>
#include <array>

static void test_row_major_2d() {
    std::array<size_t, 2> dims = {4, 5};
    gpu_layout::Layout L(dims, true);
    assert(L.is_valid());
    std::array<size_t, 2> idx = {2, 3};
    size_t off = L.offset(idx);
    assert(off == 2 * 5 + 3); // 13
}

static void test_column_major_2d() {
    std::array<size_t, 2> dims = {4, 5};
    gpu_layout::Layout L(dims, false);
    assert(L.is_valid());
    std::array<size_t, 2> idx = {2, 3};
    size_t off = L.offset(idx);
    assert(off == 3 * 4 + 2); // 14
}

static void test_transpose() {
    std::array<size_t, 2> dims = {3, 2};
    gpu_layout::Layout L(dims, true);
    std::array<size_t, 2> perm = {1, 0};
    auto T = L.transpose(perm);
    assert(T.has_value());

    std::array<size_t, 2> idx_orig = {1, 0};
    size_t off_orig = L.offset(idx_orig);

    std::array<size_t, 2> idx_t = {0, 1};
    size_t off_t = T->offset(idx_t);
    assert(off_orig == off_t);
}

static void test_reshape() {
    std::array<size_t, 2> dims = {2, 3};
    gpu_layout::Layout L(dims, true);
    std::array<size_t, 3> new_dims = {3, 2, 1};
    auto R = L.reshape(new_dims);
    assert(R.has_value());

    std::array<size_t, 2> idx_orig = {1, 2};
    size_t off_orig = L.offset(idx_orig);

    std::array<size_t, 3> idx_new = {2, 1, 0};
    size_t off_new = R->offset(idx_new);
    assert(off_orig == off_new);
}

static void test_invalid() {
    std::array<size_t, 2> dims = {0, 5};
    gpu_layout::Layout L(dims, true);
    assert(!L.is_valid());
}

int main() {
    test_row_major_2d();
    test_column_major_2d();
    test_transpose();
    test_reshape();
    test_invalid();
    std::cout << "All C++ GPU layout algebra tests passed successfully." << std::endl;
    return 0;
}
