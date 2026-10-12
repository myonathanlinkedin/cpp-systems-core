#include <vector>

#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <numeric>

using namespace cdc;

// Helper to generate deterministic pseudo‑random data.
static std::vector<uint8_t> generate_data(std::size_t size, uint32_t seed = 0xDEADBEEF) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 255);
    std::vector<uint8_t> buf(size);
    for (auto& b : buf) b = static_cast<uint8_t>(dist(rng));
    return buf;
}

// Unit test 1: empty input yields no chunks.
static void test_empty() {
    std::vector<uint8_t> data;
    auto chunks = chunk_boundaries(data);
    assert(chunks.empty());
}

// Unit test 2: single‑byte input yields one chunk of length 1.
static void test_single_byte() {
    std::vector<uint8_t> data = {0x42};
    auto chunks = chunk_boundaries(data);
    assert(chunks.size() == 1);
    assert(chunks[0].offset == 0 && chunks[0].length == 1);
}

// Unit test 3: verify reconstruction of original data.
static void test_reconstruction() {
    auto data = generate_data(123456);
    auto chunks = chunk_boundaries(data);
    std::vector<uint8_t> rebuilt;
    rebuilt.reserve(data.size());
    for (const auto& ch : chunks) {
        rebuilt.insert(rebuilt.end(),
                       data.begin() + ch.offset,
                       data.begin() + ch.offset + ch.length);
    }
    assert(rebuilt == data);
}

// Unit test 4: chunk size constraints.
static void test_size_constraints() {
    CDCParams params(1024, 4096, 8192, 48);
    auto data = generate_data(500000);
    auto chunks = chunk_boundaries(data, params);
    for (const auto& ch : chunks) {
        assert(ch.length >= params.min_size);
        assert(ch.length <= params.max_size);
    }
}

// Simple benchmark: time chunking of a 10 MiB buffer.
static void benchmark() {
    const std::size_t size = 10 * 1024 * 1024;
    auto data = generate_data(size);
    CDCParams params; // defaults
    auto start = std::chrono::high_resolution_clock::now();
    auto chunks = chunk_boundaries(data, params);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::size_t total_chunks = chunks.size();
    std::size_t avg_len = std::accumulate(chunks.begin(), chunks.end(), std::size_t(0),
                                         [](std::size_t acc, const Chunk& c){ return acc + c.length; }) / total_chunks;
    std::cout << "Benchmark: processed " << size << " bytes in "
              << elapsed.count() << " s (" << (size / (1024.0 * 1024.0) / elapsed.count())
              << " MiB/s), chunks: " << total_chunks << ", avg length: " << avg_len << " bytes.\n";
}

// Entry point runs all tests then benchmark.
int main() {
    test_empty();
    test_single_byte();
    test_reconstruction();
    test_size_constraints();
    std::cout << "All unit tests passed.\n";
    benchmark();
    return 0;
}
