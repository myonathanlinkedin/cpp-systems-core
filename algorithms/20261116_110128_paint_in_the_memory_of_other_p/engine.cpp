#include <queue>

#include "types.hpp"

void floodFill(Grid& grid, const Point& start, int newColor) {
    int rows = static_cast<int>(grid.size());
    if (rows == 0) {
        return;
    }
    int cols = static_cast<int>(grid[0].size());

    // Validate start coordinates.
    if (start.x < 0 || start.x >= rows || start.y < 0 || start.y >= cols) {
        return;
    }

    int originalColor = grid[start.x][start.y];
    if (originalColor == newColor) {
        return; // Nothing to change.
    }

    std::queue<Point> q;
    q.push(start);

    while (!q.empty()) {
        Point p = q.front();
        q.pop();

        int x = p.x;
        int y = p.y;

        if (grid[x][y] != originalColor) {
            continue;
        }

        grid[x][y] = newColor;

        // Explore the four orthogonal neighbours.
        if (x > 0) {
            q.push({x - 1, y});
        }
        if (x + 1 < rows) {
            q.push({x + 1, y});
        }
        if (y > 0) {
            q.push({x, y - 1});
        }
        if (y + 1 < cols) {
            q.push({x, y + 1});
        }
    }
}
