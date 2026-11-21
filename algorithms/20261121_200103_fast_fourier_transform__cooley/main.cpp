#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace fft {
    void fft(std::vector<std::complex<double>>& a, bool invert);
}

// Naïve DFT for verification (O(n²))
static std::vector<std::complex<double>> dft(const std::vector<std::complex<double>>& a, bool invert) {
    const std::size_t n = a.size();
    std::vector<std::complex<double>> res(n);
    const double ang_sign = invert ? 1.0 : -1.0;
    const double norm = invert ? 1.0 / static_cast<double>(n) : 1.0;

    for (std::size_t k = 0; k < n; ++k) {
        std::complex<double> sum(0.0, 0.0);
        for (std::size_t t = 0; t < n; ++t) {
            double ang = 2 * M_PI * static_cast<double>(k * t) / static_cast<double>(n);
            std::complex<double> w(std::cos(ang), std::sin(ang) * ang_sign);
            sum += a[t] * w;
        }
        res[k] = sum * norm;
    }
    return res;
}

// Helper to generate random complex vector of size power‑of‑two
static std::vector<std::complex<double>> random_complex_vector(std::size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    std::vector<std::complex<double>> v(n);
    for (auto& x : v) {
        x = std::complex<double>(dist(rng), dist(rng));
    }
    return v;
}

// Compare two vectors element‑wise within tolerance
static void assert_vectors_close(const std::vector<std::complex<double>>& a,
                                 const std::vector<std::complex<double>>& b,
                                 double eps = 1e-9) {
    assert(a.size() == b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        double err = std::abs(a[i] - b[i]);
        assert(err <= eps && "Vectors differ beyond tolerance");
    }
}

int main() {
    // Test 1: size 1 (trivial)
    {
        std::vector<std::complex<double>> v = { {0.5, -0.3} };
        std::vector<std::complex<double>> expected = dft(v, false);
        fft::fft(v, false);
        assert_vectors_close(v, expected);
        fft::fft(v, true); // inverse should recover original
        assert_vectors_close(v, expected);
    }

    // Test 2: size 2
    {
        std::vector<std::complex<double>> v = { {1.0, 0.0}, {0.0, 1.0} };
        std::vector<std::complex<double>> expected = dft(v, false);
        fft::fft(v, false);
        assert_vectors_close(v, expected);
        fft::fft(v, true);
        assert_vectors_close(v, expected);
    }

    // Test 3: random size 8, compare with naïve DFT
    {
        const std::size_t n = 8;
        auto v = random_complex_vector(n);
        auto expected = dft(v, false);
        fft::fft(v, false);
        assert_vectors_close(v, expected);
    }

    // Test 4: forward + inverse recovers original (size 16)
    {
        const std::size_t n = 16;
        auto original = random_complex_vector(n);
        auto work = original;
        fft::fft(work, false); // forward
        fft::fft(work, true);  // inverse
        assert_vectors_close(work, original);
    }

    // Test 5: larger size 64, sanity check against DFT (still feasible)
    {
        const std::size_t n = 64;
        auto v = random_complex_vector(n);
        auto expected = dft(v, false);
        fft::fft(v, false);
        assert_vectors_close(v, expected, 1e-8);
    }

    std::cout << "All FFT unit tests passed.\n";
    return 0;
}
