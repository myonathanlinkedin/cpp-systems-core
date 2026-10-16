#include <map>
#include <set>
#include <utility>

#pragma once
#include "types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace lsm {

class LSMTree {
public:
    explicit LSMTree(size_t memtable_threshold = 100);
    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    void flush();
    void compact();

private:
    size_t memtable_threshold_;
    std::unordered_map<std::string, std::string> memtable_;
    std::vector<std::vector<std::pair<std::string, std::string>>> sstables_;
};

} // namespace lsm
