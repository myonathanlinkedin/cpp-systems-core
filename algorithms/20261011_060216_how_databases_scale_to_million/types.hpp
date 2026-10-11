#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace lsm {

struct KeyValue {
    std::string key;
    std::string value;
    KeyValue() = default;
    KeyValue(std::string k, std::string v) : key(std::move(k)), value(std::move(v)) {};
};

} // namespace lsm
