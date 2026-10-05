#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <cctype>
#include <functional>
#include <queue>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <limits>
#include <stdexcept>

namespace orama {

// 1. Tokenizer: Normalizes and splits text into tokens
class Tokenizer {
public:
    static std::vector<std::string> tokenize(const std::string& text) {
        std::vector<std::string> tokens;
        std::string current;
        for (char c : text) {
            if (std::isalnum(c)) {
                current += static_cast<char>(std::tolower(c));
            } else {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            }
        }
        if (!current.empty()) tokens.push_back(current);
        return tokens;
    }
};

// 2. Inverted Index: Maps terms to document IDs
class InvertedIndex {
    std::unordered_map<std::string, std::unordered_set<int>> index_;
public:
    void addDocument(int docId, const std::string& text) {
        auto tokens = Tokenizer::tokenize(text);
        for (const auto& token : tokens) {
            index_[token].insert(docId);
        }
    }

    std::vector<int> search(const std::string& query) const {
        auto tokens = Tokenizer::tokenize(query);
        std::unordered_set<int> result;
        for (const auto& token : tokens) {
            auto it = index_.find(token);
            if (it != index_.end()) {
                for (int docId : it->second) {
                    result.insert(docId);
                }
            }
        }
        return std::vector<int>(result.begin(), result.end());
    }

    size_t size() const { return index_.size(); }
};

// 3. Vector Store: Stores embeddings for semantic search
class VectorStore {
    std::vector<std::vector<float>> vectors_;
    std::vector<int> docIds_;
public:
    void addVector(const std::vector<float>& vec, int docId) {
        vectors_.push_back(vec);
        docIds_.push_back(docId);
    }

    // Cosine Similarity Search
    std::vector<std::pair<int, float>> search(const std::vector<float>& queryVec, int topK) const {
        std::vector<std::pair<float, int>> scores;
        for (size_t i = 0; i < vectors_.size(); ++i) {
            float dot = 0.0f, normA = 0.0f, normB = 0.0f;
            for (size_t j = 0; j < queryVec.size(); ++j) {
                dot += queryVec[j] * vectors_[i][j];
                normA += queryVec[j] * queryVec[j];
                normB += vectors_[i][j] * vectors_[i][j];
            }
            float sim = (normA > 0 && normB > 0) ? dot / (std::sqrt(normA) * std::sqrt(normB)) : 0.0f;
            scores.push_back({sim, docIds_[i]});
        }
        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });
        
        std::vector<std::pair<int, float>> results;
        for (int i = 0; i < topK && i < static_cast<int>(scores.size()); ++i) {
            results.push_back({scores[i].second, scores[i].first});
        }
        return results;
    }
    
    size_t size() const { return vectors_.size(); }
};

// 4. RAG Pipeline: Combines search and retrieval
class RAGPipeline {
    InvertedIndex keywordIndex_;
    VectorStore vectorStore_;
    std::unordered_map<int, std::string> documents_;
    int nextDocId_ = 0;

public:
    int addDocument(const std::string& content, const std::vector<float>& embedding) {
        int id = nextDocId_++;
        documents_[id] = content;
        keywordIndex_.addDocument(id, content);
        vectorStore_.addVector(embedding, id);
        return id;
    }

    // Hybrid Search: Combines keyword and vector results
    std::vector<std::pair<int, float>> hybridSearch(const std::string& query, const std::vector<float>& queryVec, int topK) const {
        // 1. Keyword Search
        auto keywordResults = keywordIndex_.search(query);
        std::unordered_map<int, float> scores;
        for (int id : keywordResults) {
            scores[id] = 1.0f; // Base score for keyword match
        }

        // 2. Vector Search
        auto vectorResults = vectorStore_.search(queryVec, topK * 2);
        for (const auto& [id, sim] : vectorResults) {
            scores[id] += sim; // Add similarity score
        }

        // 3. Sort by combined score
        std::vector<std::pair<int, float>> results(scores.begin(), scores.end());
        std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        if (results.size() > static_cast<size_t>(topK)) {
            results.resize(topK);
        }
        return results;
    }

    std::string getDocument(int id) const {
        auto it = documents_.find(id);
        return (it != documents_.end()) ? it->second : "";
    }
};

} // namespace orama
