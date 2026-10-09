#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>
#include <map>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

// ============================================================================
// Domain Models for Attention via Black-Box Vector Search
// ============================================================================
//
// This module implements a "black-box" vector search engine that supports
// approximate nearest neighbor (ANN) queries using a locality-sensitive
// hashing (LSH) based index, combined with exact brute-force fallback.
//
// The "attention" metaphor: given a query vector, the engine retrieves the
// top-k most similar vectors from a corpus, weighted by cosine similarity,
// producing a weighted sum (the "attention output").
//
// Complexity invariants:
//   - Index build: O(N * D * L) where L = number of hash tables
//   - Query (ANN): O(N / B * D) expected, where B = bucket size
//   - Query (exact): O(N * D)
//   - Attention output: O(k * D)
// ============================================================================

namespace attention_search {

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
struct Config {
    int dim = 64;              // vector dimensionality
    int numHashTables = 10;    // L = number of independent hash tables
    int numBuckets = 256;      // B = buckets per table (power of 2)
    int topK = 5;              // k = number of neighbors to retrieve
    double similarityThreshold = 0.0; // minimum cosine similarity to include
    bool useExactFallback = true;    // if ANN returns < k, fill with exact
    unsigned int seed = 42;     // deterministic RNG seed
};

// ---------------------------------------------------------------------------
// Vector type: fixed-size dense vector stored as std::vector<double>
// ---------------------------------------------------------------------------
struct Vector {
    std::vector<double> data;

    Vector() = default;
    explicit Vector(int dim) : data(dim, 0.0) {};
    Vector(std::vector<double> d) : data(std::move(d)) {}

    int size() const { return static_cast<int>(data.size()); }
    double& operator[](int i) { return data[i]; }
    const double& operator[](int i) const { return data[i]; }

    // L2 norm
    double norm() const {
        double sum = 0.0;
        for (double v : data) sum += v * v;
        return std::sqrt(sum);
    }

    // Dot product
    double dot(const Vector& other) const {
        double sum = 0.0;
        for (int i = 0; i < size(); ++i) sum += data[i] * other.data[i];
        return sum;
    }

    // Cosine similarity in [-1, 1]
    double cosineSimilarity(const Vector& other) const {
        double dot = this->dot(other);
        double n1 = this->norm();
        double n2 = other.norm();
        if (n1 < 1e-12 || n2 < 1e-12) return 0.0;
        return dot / (n1 * n2);
    }

    // L2 distance
    double l2Distance(const Vector& other) const {
        double sum = 0.0;
        for (int i = 0; i < size(); ++i) {
            double d = data[i] - other.data[i];
            sum += d * d;
        }
        return std::sqrt(sum);
    }

    // Normalized copy (unit vector)
    Vector normalized() const {
        double n = norm();
        Vector result(size());
        if (n < 1e-12) return result;
        for (int i = 0; i < size(); ++i) result.data[i] = data[i] / n;
        return result;
    }

    bool operator==(const Vector& other) const {
        if (size() != other.size()) return false;
        for (int i = 0; i < size(); ++i) {
            if (std::abs(data[i] - other.data[i]) > 1e-12) return false;
        }
        return true;
    }
};

// ---------------------------------------------------------------------------
// Scored result: a vector index paired with its similarity score
// ---------------------------------------------------------------------------
struct ScoredResult {
    int index = -1;
    double score = 0.0;

    ScoredResult() = default;
    ScoredResult(int idx, double s) : index(idx), score(s) {};

    bool operator<(const ScoredResult& other) const {
        return score < other.score; // for max-heap
    }
};

// ---------------------------------------------------------------------------
// Attention output: weighted sum of top-k neighbors
// ---------------------------------------------------------------------------
struct AttentionOutput {
    Vector weightedSum;
    std::vector<ScoredResult> topResults;
    double totalWeight = 0.0;

    AttentionOutput() = default;
};

// ---------------------------------------------------------------------------
// LSH Hash Table entry
// ---------------------------------------------------------------------------
struct LSHBucket {
    std::vector<int> indices; // indices into the corpus
};

// ---------------------------------------------------------------------------
// LSH Index: multiple hash tables for ANN
// ---------------------------------------------------------------------------
class LSHIndex {
public:
    LSHIndex() = default;

    void build(const std::vector<Vector>& corpus, const Config& cfg);
    std::vector<int> query(const Vector& q, int k) const;

    int numTables() const { return static_cast<int>(hashTables_.size()); };
    int numBuckets() const { return numBuckets_; }
    int dim() const { return dim_; }

private:
    // Hash a vector into a bucket using random hyperplane projection
    int hashVector(const Vector& v, int tableIdx) const;

    int dim_ = 0;
    int numBuckets_ = 0;
    std::vector<std::vector<std::vector<double>>> hyperplanes_; // [table][dim]
    std::vector<std::vector<LSHBucket>> hashTables_; // [table][bucket]
};

// ---------------------------------------------------------------------------
// Black-Box Vector Search Engine
// ---------------------------------------------------------------------------
class VectorSearchEngine {
public:
    VectorSearchEngine() = default;
    explicit VectorSearchEngine(const Config& cfg) : config_(cfg) {};

    // Build the index from a corpus of vectors
    void build(const std::vector<Vector>& corpus);

    // Query: retrieve top-k most similar vectors
    // Returns vector of ScoredResult sorted by descending score
    std::vector<ScoredResult> query(const Vector& q, int k = -1) const;

    // Attention: compute weighted sum of top-k neighbors
    AttentionOutput attention(const Vector& q, int k = -1) const;

    // Exact brute-force search (for verification / fallback)
    std::vector<ScoredResult> exactQuery(const Vector& q, int k = -1) const;

    // Corpus access
    int corpusSize() const { return static_cast<int>(corpus_.size()); }
    const Vector& getVector(int idx) const { return corpus_[idx]; }
    const Config& getConfig() const { return config_; }

    // Statistics
    struct Stats {
        int corpusSize = 0;
        int numTables = 0;
        int numBuckets = 0;
        double avgBucketSize = 0.0;
        double maxBucketSize = 0.0;
    };
    Stats getStats() const;

private:
    Config config_;
    std::vector<Vector> corpus_;
    LSHIndex lshIndex_;
};

// ---------------------------------------------------------------------------
// Test data structures
// ---------------------------------------------------------------------------
struct TestData {
    std::vector<Vector> corpus;
    Vector query;
    int expectedTopK;
    double expectedTopScore;
    std::string description;

    TestData() = default;
};

} // namespace attention_search
