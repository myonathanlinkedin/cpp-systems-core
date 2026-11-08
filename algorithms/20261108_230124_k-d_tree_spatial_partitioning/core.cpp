#include <vector>
#include <memory>
#include <numeric>
#include <functional>
#include <stdexcept>

#include "core.hpp"

namespace kd {

KdTree::KdTree(size_t dimensions) : root_(nullptr), k_(dimensions) {
    if (dimensions == 0) {
        throw std::invalid_argument("Dimension must be positive");
    }
}

void KdTree::build(const std::vector<Point>& points) {
    if (points.empty()) {
        root_.reset();
        return;
    }
    for (const auto& p : points) {
        if (p.coords.size() != k_) {
            throw std::invalid_argument("Point dimension mismatch");
        }
    }
    std::vector<Point> pts = points;
    root_ = buildRecursive(pts.begin(), pts.end(), 0);
}

std::unique_ptr<KdTree::Node> KdTree::buildRecursive(std::vector<Point>::iterator begin,
                                                     std::vector<Point>::iterator end,
                                                     size_t depth) {
    if (begin >= end) {
        return nullptr;
    }
    size_t axis = depth % k_;
    auto mid = begin + (end - begin) / 2;
    std::nth_element(begin, mid, end,
                     [axis](const Point& a, const Point& b) {
                         return a.coords[axis] < b.coords[axis];
                     });
    auto node = std::make_unique<Node>(*mid, axis);
    node->left = buildRecursive(begin, mid, depth + 1);
    node->right = buildRecursive(mid + 1, end, depth + 1);
    return node;
}

void KdTree::insert(const Point& point) {
    if (point.coords.size() != k_) {
        throw std::invalid_argument("Point dimension mismatch");
    }
    std::function<std::unique_ptr<Node>(std::unique_ptr<Node>&, const Point&, size_t)> insertRec;
    insertRec = [&](std::unique_ptr<Node>& cur, const Point& pt, size_t depth) -> std::unique_ptr<Node> {
        if (!cur) {
            return std::make_unique<Node>(pt, depth % k_);
        }
        size_t axis = cur->axis;
        if (pt.coords[axis] < cur->point.coords[axis]) {
            cur->left = insertRec(cur->left, pt, depth + 1);
        } else {
            cur->right = insertRec(cur->right, pt, depth + 1);
        }
        return std::move(cur);
    };
    root_ = insertRec(root_, point, 0);
}

const Point* KdTree::nearest(const Point& query) const {
    if (!root_) {
        return nullptr;
    }
    if (query.coords.size() != k_) {
        throw std::invalid_argument("Query dimension mismatch");
    }
    const Point* best = nullptr;
    double bestDistSq = std::numeric_limits<double>::infinity();
    nearestRecursive(root_.get(), query, best, bestDistSq);
    return best;
}

void KdTree::nearestRecursive(const Node* node, const Point& query,
                              const Point*& best, double& bestDistSq) const {
    if (!node) return;
    double distSq = distanceSquared(node->point, query);
    if (distSq < bestDistSq) {
        bestDistSq = distSq;
        best = &node->point;
    }
    size_t axis = node->axis;
    double diff = query.coords[axis] - node->point.coords[axis];
    const Node* first = diff < 0 ? node->left.get() : node->right.get();
    const Node* second = diff < 0 ? node->right.get() : node->left.get();

    nearestRecursive(first, query, best, bestDistSq);
    if (diff * diff < bestDistSq) {
        nearestRecursive(second, query, best, bestDistSq);
    }
}

} // namespace kd
