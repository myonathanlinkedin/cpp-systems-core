#include "core.hpp"
#include <numeric>
#include <stdexcept>
#include <cassert>

namespace gpu_layout {

void Layout::compute_strides() {
    if (desc_.ndim == 0) return;
    desc_.strides.resize(desc_.ndim);
    if (desc_.row_major) {
        desc_.strides[desc_.ndim - 1] = 1;
        for (long long i = static_cast<long long>(desc_.ndim) - 2; i >= 0; --i) {
            desc_.strides[i] = desc_.strides[i + 1] * desc_.dims[i + 1];
        }
    } else {
        desc_.strides[0] = 1;
        for (size_t i = 1; i < desc_.ndim; ++i) {
            desc_.strides[i] = desc_.strides[i - 1] * desc_.dims[i - 1];
        }
    }
}

Layout::Layout(std::span<const size_t> dims, bool row_major) {
    desc_.ndim = dims.size();
    desc_.dims.assign(dims.begin(), dims.end());
    desc_.row_major = row_major;
    compute_strides();
}

Layout::Layout(const LayoutDescriptor& desc) : desc_(desc) {}

bool Layout::is_valid() const noexcept {
    if (desc_.ndim == 0 || desc_.dims.size() != desc_.ndim || desc_.strides.size() != desc_.ndim) {
        return false;
    }
    for (size_t d : desc_.dims) {
        if (d == 0) return false;
    }
    return true;
}

size_t Layout::offset(std::span<const size_t> indices) const {
    if (indices.size() != desc_.ndim) {
        throw std::invalid_argument("Indices size mismatch with layout dimensions");
    }
    size_t off = 0;
    for (size_t i = 0; i < desc_.ndim; ++i) {
        if (indices[i] >= desc_.dims[i]) {
            throw std::out_of_range("Index out of bound for dimension");
        }
        off += indices[i] * desc_.strides[i];
    }
    return off;
}

std::optional<Layout> Layout::transpose(std::span<const size_t> perm) const {
    if (perm.size() != desc_.ndim) return std::nullopt;
    LayoutDescriptor td;
    td.ndim = desc_.ndim;
    td.row_major = desc_.row_major;
    td.dims.resize(td.ndim);
    td.strides.resize(td.ndim);

    std::vector<bool> seen(td.ndim, false);
    for (size_t i = 0; i < td.ndim; ++i) {
        size_t p = perm[i];
        if (p >= td.ndim || seen[p]) return std::nullopt;
        seen[p] = true;
        td.dims[i] = desc_.dims[p];
        td.strides[i] = desc_.strides[p];
    }
    return Layout(td);
}

std::optional<Layout> Layout::reshape(std::span<const size_t> new_dims) const {
    if (new_dims.empty()) return std::nullopt;
    size_t old_total = 1;
    for (size_t d : desc_.dims) old_total *= d;

    size_t new_total = 1;
    for (size_t d : new_dims) {
        if (d == 0) return std::nullopt;
        new_total *= d;
    }
    if (old_total != new_total) return std::nullopt;

    return Layout(new_dims, desc_.row_major);
}

} // namespace gpu_layout
