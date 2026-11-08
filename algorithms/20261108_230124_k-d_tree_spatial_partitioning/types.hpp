#pragma once
#include <vector>
#include <cstddef>
#include <cmath>
#include <limits>

namespace kd {

struct Point {
    std::vector<double> coords;
    Point() = default;
    explicit Point(const std::vector<double>& c) : coords(c) {};
    explicit Point(std::vector<double>&& c) : coords(std::move(c)) {}
};

inline double distanceSquared(const Point& a, const Point& b) {
    double dist = 0.0;
    for (size_t i = 0; i < a.coords.size(); ++i) {
        double diff = a.coords[i] - b.coords[i];
        dist += diff * diff;
    }
    return dist;
}

} // namespace kd
