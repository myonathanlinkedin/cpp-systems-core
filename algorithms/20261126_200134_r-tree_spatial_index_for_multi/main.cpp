#include <iostream>
#include <vector>
#include <cassert>
#include "core.cpp"

int main() {
    RTree tree(4);

    // Insert 10 unit squares along x-axis
    for (int i = 0; i < 10; ++i)
        tree.insert(Rect(i, 0, i + 1, 1));

    // Search for a single rectangle
    auto res1 = tree.search(Rect(5, 0, 6, 1));
    assert(res1.size() == 1);
    assert(res1[0].xmin == 5 && res1[0].xmax == 6);

    // Search for all rectangles
    auto res2 = tree.search(Rect(0, 0, 10, 1));
    assert(res2.size() == 10);

    // Search for empty region
    auto res3 = tree.search(Rect(20, 0, 30, 1));
    assert(res3.empty());

    // Insert more to trigger splits
    for (int i = 10; i < 30; ++i)
        tree.insert(Rect(i, 0, i + 1, 1));
    auto res4 = tree.search(Rect(0, 0, 30, 1));
    assert(res4.size() == 30);

    std::cout << "All tests passed.\n";
    return 0;
}
