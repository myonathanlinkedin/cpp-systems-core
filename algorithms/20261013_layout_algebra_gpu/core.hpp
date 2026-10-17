#pragma once
#include "types.hpp"
#include <optional>
#include <vector>
#include <span>

namespace gpu_layout {

class Layout {
public:
    Layout(std::span<const size_t> dims, bool row_major = true);
    explicit Layout(const LayoutDescriptor& desc);

    [[nodiscard]] size_t ndim() const noexcept { return desc_.ndim; }
    [[nodiscard]] const std::vector<size_t>& dims() const noexcept { return desc_.dims; }
    [[nodiscard]] const std::vector<size_t>& strides() const noexcept { return desc_.strides; }
    [[nodiscard]] bool is_row_major() const noexcept { return desc_.row_major; }

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] size_t offset(std::span<const size_t> indices) const;
    [[nodiscard]] std::optional<Layout> transpose(std::span<const size_t> perm) const;
    [[nodiscard]] std::optional<Layout> reshape(std::span<const size_t> new_dims) const;

    [[nodiscard]] const LayoutDescriptor& descriptor() const noexcept { return desc_; }

private:
    LayoutDescriptor desc_;
    void compute_strides();
};

} // namespace gpu_layout
