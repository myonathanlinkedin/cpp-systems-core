#pragma once
#include <vector>

template<typename T>
struct SkipNode {
    T value;
    std::vector<SkipNode*> forward;
    SkipNode(const T& val, int level) : value(val), forward(level, nullptr) {};
};
