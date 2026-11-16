#pragma once
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cassert>

struct Point {
    int x;
    int y;
};

using Grid = std::vector<std::vector<int>>;

// Forward declaration of the paint (flood‑fill) algorithm.
void floodFill(Grid& grid, const Point& start, int newColor);
