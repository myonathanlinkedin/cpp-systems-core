#include <vector>
#include <cassert>

#include <iostream>
#include "types.hpp"
#include "engine.cpp"

int main() {
    // Test 1: Single cell repaint.
    Grid g1 = {
        {1, 1, 1},
        {1, 2, 1},
        {1, 1, 1}
    };
    floodFill(g1, {1, 1}, 3);
    Grid expected1 = {
        {1, 1, 1},
        {1, 3, 1},
        {1, 1, 1}
    };
    assert(g1 == expected1);

    // Test 2: Fill a rectangular region.
    Grid g2 = {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    };
    floodFill(g2, {1, 1}, 2);
    Grid expected2 = {
        {0, 0, 0, 0},
        {0, 2, 2, 0},
        {0, 2, 2, 0},
        {0, 0, 0, 0}
    };
    assert(g2 == expected2);

    // Test 3: Starting point out of bounds – grid must remain unchanged.
    Grid g3 = {{5}};
    floodFill(g3, {-1, 0}, 9);
    assert(g3[0][0] == 5);

    // Test 4: New colour equals original colour – no modification.
    Grid g4 = {{7, 7}, {7, 7}};
    floodFill(g4, {0, 0}, 7);
    Grid expected4 = {{7, 7}, {7, 7}};
    assert(g4 == expected4);

    // Test 5: Large grid with a diagonal barrier.
    const int N = 100;
    Grid g5(N, std::vector<int>(N, 0));
    for (int i = 0; i < N; ++i) {
        g5[i][i] = 1; // barrier cells.
    }
    floodFill(g5, {0, N - 1}, 2);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (j > i) {
                assert(g5[i][j] == 2); // region above the barrier painted.
            } else if (j < i) {
                assert(g5[i][j] == 0); // region below remains untouched.
            } else {
                assert(g5[i][j] == 1); // barrier unchanged.
            }
        }
    }

    std::cout << "All tests passed.\n";
    return 0;
}
