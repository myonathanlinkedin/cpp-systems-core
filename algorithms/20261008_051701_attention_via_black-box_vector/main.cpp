#include <numeric>
#include <cmath>

#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>

void test_dot() {
    Vector a{1.0, 2.0, 3.0};
    Vector b{4.0, 5.0, 6.0};
    double d = dot(a, b)[0];
    assert(std::abs(d - 32.0) < 1e-9);
    Vector c{1.0, 0.0, -1.0};
    double d2 = dot(a, c)[0];
    assert(std::abs(d2 - 0.0) < 1e-9);
}

void test_softmax() {
    Vector s{0.0, 0.0};
    Vector w = softmax(s);
    assert(std::abs(w[0] - 0.5) < 1e-9);
    assert(std::abs(w[1] - 0.5) < 1e-9);
    Vector s2{1.0, 2.0, 3.0};
    Vector w2 = softmax(s2);
    double sum = std::accumulate(w2.begin(), w2.end(), 0.0);
    assert(std::abs(sum - 1.0) < 1e-9);
}

void test_attention_single() {
    Vector query{1.0, 0.0};
    Matrix keys{{1.0, 0.0}};
    Matrix values{{0.5, 0.5}};
    Vector out = attention(query, keys, values);
    assert(out.size() == 2);
    assert(std::abs(out[0] - 0.5) < 1e-9);
    assert(std::abs(out[1] - 0.5) < 1e-9);
}

void test_attention_multiple() {
    Vector query{1.0, 0.0};
    Matrix keys{{1.0, 0.0}, {0.0, 1.0}};
    Matrix values{{1.0, 0.0}, {0.0, 1.0}};
    Vector out = attention(query, keys, values);
    // Expected weights ~ [0.731, 0.269]
    double w0 = 0.7310585786300049;
    double w1 = 0.2689414213699951;
    assert(std::abs(out[0] - w0) < 1e-6);
    assert(std::abs(out[1] - w1) < 1e-6);
}

void test_attention_zero_query() {
    Vector query{0.0, 0.0};
    Matrix keys{{1.0, 0.0}, {0.0, 1.0}};
    Matrix values{{1.0, 0.0}, {0.0, 1.0}};
    Vector out = attention(query, keys, values);
    // All similarities zero -> equal weights 0.5
    assert(std::abs(out[0] - 0.5) < 1e-9);
    assert(std::abs(out[1] - 0.5) < 1e-9);
}

void test_attention_empty() {
    Vector query{1.0, 2.0};
    Matrix keys{};
    Matrix values{};
    Vector out = attention(query, keys, values);
    assert(out.empty());
}

int main() {
    test_dot();
    test_softmax();
    test_attention_single();
    test_attention_multiple();
    test_attention_zero_query();
    test_attention_empty();
    std::cout << "All tests passed.\n";
    return 0;
}
