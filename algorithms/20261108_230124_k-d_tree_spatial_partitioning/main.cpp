#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>
#include <vector>

int main() {
    using namespace kd;

    // Test 1: Empty tree
    {
        KdTree tree(2);
        const Point* result = tree.nearest(Point{{0.0, 0.0}});
        assert(result == nullptr);
    }

    // Test 2: Single point
    {
        KdTree tree(3);
        Point p({1.0, 2.0, 3.0});
        tree.build({p});
        const Point* nn = tree.nearest(Point{{1.1, 2.1, 3.1}});
        assert(nn != nullptr);
        assert(nn->coords == p.coords);
    }

    // Test 3: Multiple points, known nearest
    {
        std::vector<Point> pts = {
            Point({0.0, 0.0}),
            Point({5.0, 5.0}),
            Point({2.0, 1.0}),
            Point({-1.0, -1.0}),
            Point({3.0, 4.0})
        };
        KdTree tree(2);
        tree.build(pts);
        Point query({2.1, 1.1});
        const Point* nn = tree.nearest(query);
        assert(nn != nullptr);
        assert(nn->coords[0] == 2.0 && nn->coords[1] == 1.0);
    }

    // Test 4: Duplicate points
    {
        std::vector<Point> pts = {
            Point({0.0, 0.0}),
            Point({0.0, 0.0}),
            Point({1.0, 1.0})
        };
        KdTree tree(2);
        tree.build(pts);
        Point query({0.0, 0.0});
        const Point* nn = tree.nearest(query);
        assert(nn != nullptr);
        assert(nn->coords[0] == 0.0 && nn->coords[1] == 0.0);
    }

    // Test 5: Higher dimension (4D)
    {
        std::vector<Point> pts = {
            Point({1.0, 2.0, 3.0, 4.0}),
            Point({-1.0, -2.0, -3.0, -4.0}),
            Point({0.5, 0.5, 0.5, 0.5})
        };
        KdTree tree(4);
        tree.build(pts);
        Point query({0.6, 0.6, 0.6, 0.6});
        const Point* nn = tree.nearest(query);
        assert(nn != nullptr);
        assert(nn->coords[0] == 0.5);
    }

    std::cout << "All KdTree unit tests passed.\n";
    return 0;
}
