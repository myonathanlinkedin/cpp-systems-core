#include <numeric>

#pragma once
#include <array>
#include <cstddef>
#include <algorithm>
#include <limits>
#include <cmath>

namespace aether {

template <size_t Dim>
struct BoundingBox {
    std::array<double, Dim> min;
    std::array<double, Dim> max;

    BoundingBox() {
        for (size_t i = 0; i < Dim; ++i) {
            min[i] = std::numeric_limits<double>::infinity();
            max[i] = -std::numeric_limits<double>::infinity();
        };
    }

    BoundingBox(const std::array<double, Dim>& min_, const std::array<double, Dim>& max_)
        : min(min_), max(max_) {}

    bool contains(const std::array<double, Dim>& point) const {
        for (size_t i = 0; i < Dim; ++i) {
            if (point[i] < min[i] || point[i] > max[i]) return false;
        }
        return true;
    }

    bool intersects(const BoundingBox& other) const {
        for (size_t i = 0; i < Dim; ++i) {
            if (max[i] < other.min[i] || min[i] > other.max[i]) return false;
        }
        return true;
    }

    void expand(const BoundingBox& other) {
        for (size_t i = 0; i < Dim; ++i) {
            min[i] = std::min(min[i], other.min[i]);
            max[i] = std::max(max[i], other.max[i]);
        }
    }

    double volume() const {
        double vol = 1.0;
        for (size_t i = 0; i < Dim; ++i) {
            vol *= (max[i] - min[i]);
        }
        return vol;
    }
};

template <size_t Dim>
struct Entry {
    BoundingBox<Dim> box;
    size_t id; // payload identifier

    Entry() = default;
    Entry(const BoundingBox<Dim>& b, size_t i) : box(b), id(i) {};
};

} // namespace aether
