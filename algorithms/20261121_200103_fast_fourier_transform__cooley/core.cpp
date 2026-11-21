#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <vector>
#include <complex>
#include <cmath>
#include <cassert>

namespace fft {

inline bool is_power_of_two(std::size_t n) {
    return n && ( (n & (n - 1)) == 0 );
}

// In-place Cooley‑Tukey radix‑2 FFT.
// If invert == true, computes the inverse FFT (scaled by 1/n).
inline void fft(std::vector<std::complex<double>>& a, bool invert) {
    const std::size_t n = a.size();
    assert(is_power_of_two(n) && "FFT size must be a power of two");

    // Bit‑reversal permutation
    for (std::size_t i = 1, j = 0; i < n; ++i) {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;

        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }

    // Iterative Danielson‑Lanczos section
    for (std::size_t len = 2; len <= n; len <<= 1) {
        const double ang = 2 * M_PI / static_cast<double>(len) * (invert ? -1 : 1);
        const std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (std::size_t i = 0; i < n; i += len) {
            std::complex<double> w(1.0, 0.0);
            for (std::size_t j = 0; j < len / 2; ++j) {
                const std::complex<double> u = a[i + j];
                const std::complex<double> v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        const double inv_n = 1.0 / static_cast<double>(n);
        for (auto& x : a) {
            x *= inv_n;
        }
    }
}

} // namespace fft
