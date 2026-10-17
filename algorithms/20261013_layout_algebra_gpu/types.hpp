#pragma once
#include <cstddef>
#include <vector>
#include <string>

namespace gpu_layout {

struct LayoutDescriptor {
    size_t ndim{0};
    std::vector<size_t> dims{};
    std::vector<size_t> strides{};
    bool row_major{true};
};

} // namespace gpu_layout
