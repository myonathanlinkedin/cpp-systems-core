#include <functional>
#include <utility>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace lsm {

// Simple greeting function.
inline std::string greet() {
    return "Hello, Algo!";
}

// Fast doubling method for Fibonacci numbers (O(log n)).
// Returns F(n) where F(0)=0, F(1)=1.
inline uint64_t fibonacci(uint64_t n) {
    // Helper lambda returning pair (F(k), F(k+1)).
    std::function<std::pair<uint64_t, uint64_t>(uint64_t)> fib_pair =
        [&](uint64_t k) -> std::pair<uint64_t, uint64_t> {
            if (k == 0) return {0, 1};
            auto p = fib_pair(k >> 1);
            uint64_t a = p.first;   // F(k)
            uint64_t b = p.second;  // F(k+1)
            uint64_t c = a * ( (b << 1) - a );          // F(2k)
            uint64_t d = a * a + b * b;                // F(2k+1)
            if ((k & 1) == 0) {
                return {c, d};
            } else {
                return {d, c + d};
            }
        };
    return fib_pair(n).first;
}

// In‑place stable sort of an integer vector using std::stable_sort.
inline void stable_sort_ints(std::vector<int>& data) {
    std::stable_sort(data.begin(), data.end());
}

// Reverse a vector in‑place using std::reverse.
template <typename T>
inline void reverse_vector(std::vector<T>& data) {
    std::reverse(data.begin(), data.end());
}

// Compute the maximum subarray sum (Kadane's algorithm) for a vector of ints.
inline int max_subarray_sum(const std::vector<int>& data) {
    if (data.empty()) return 0;
    int max_ending_here = data[0];
    int max_so_far = data[0];
    for (size_t i = 1; i < data.size(); ++i) {
        max_ending_here = std::max(data[i], max_ending_here + data[i]);
        max_so_far = std::max(max_so_far, max_ending_here);
    }
    return max_so_far;
}

// Binary search on a sorted vector; returns index or -1 if not found.
template <typename T>
inline int binary_search(const std::vector<T>& sorted, const T& key) {
    size_t left = 0;
    size_t right = sorted.size();
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (sorted[mid] == key) {
            return static_cast<int>(mid);
        } else if (sorted[mid] < key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return -1;
}

} // namespace lsm
