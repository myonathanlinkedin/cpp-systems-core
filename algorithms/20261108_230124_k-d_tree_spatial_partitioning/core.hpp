#pragma once
#include "types.hpp"
#include <memory>
#include <algorithm>
#include <vector>
#include <functional>
#include <stdexcept>

namespace kd {

class KdTree {
public:
    explicit KdTree(size_t dimensions);
    void build(const std::vector<Point>& points);
    void insert(const Point& point);
    const Point* nearest(const Point& query) const;
private:
    struct Node {
        Point point;
        size_t axis;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node(const Point& pt, size_t ax) : point(pt), axis(ax), left(nullptr), right(nullptr) {};
    };
    std::unique_ptr<Node> root_;
    size_t k_;

    std::unique_ptr<Node> buildRecursive(std::vector<Point>::iterator begin,
                                         std::vector<Point>::iterator end,
                                         size_t depth);
    void nearestRecursive(const Node* node, const Point& query,
                          const Point*& best, double& bestDistSq) const;
};

} // namespace kd
