#include <iostream>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <algorithm>

enum class VCRelation { Less, Equal, Greater, Concurrent };

class VectorClock {
public:
    // Increment logical clock for process pid
    void tick(int pid) {
        ++clock_[pid];
    }

    // Merge another clock into this one (element-wise max)
    void merge(const VectorClock& other) {
        for (const auto& [pid, ts] : other.clock_) {
            auto it = clock_.find(pid);
            if (it == clock_.end() || ts > it->second)
                clock_[pid] = ts;
        }
    }

    // Compare this clock with another
    VCRelation compare(const VectorClock& other) const {
        bool less = false, greater = false;
        // Union of keys
        for (const auto& kv : clock_) {
            int pid = kv.first;
            int a = kv.second;
            int b = other.get(pid);
            if (a < b) less = true;
            else if (a > b) greater = true;
        }
        for (const auto& kv : other.clock_) {
            int pid = kv.first;
            if (clock_.find(pid) != clock_.end()) continue;
            int b = kv.second;
            int a = 0;
            if (a < b) less = true;
            else if (a > b) greater = true;
        }
        if (!less && !greater) return VCRelation::Equal;
        if (less && !greater) return VCRelation::Less;
        if (greater && !less) return VCRelation::Greater;
        return VCRelation::Concurrent;
    }

    // String representation for debugging
    std::string to_string() const {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& [pid, ts] : clock_) {
            if (!first) oss << ", ";
            oss << pid << ":" << ts;
            first = false;
        }
        oss << "}";
        return oss.str();
    }

private:
    int get(int pid) const {
        auto it = clock_.find(pid);
        return it != clock_.end() ? it->second : 0;
    }
    std::unordered_map<int, int> clock_;
};

inline std::ostream& operator<<(std::ostream& os, const VectorClock& vc) {
    return os << vc.to_string();
}

// Unit tests
int main() {
    // Test 1: Simple tick and compare
    VectorClock a, b;
    a.tick(1); // a: {1:1}
    assert(a.compare(b) == VCRelation::Greater);
    assert(b.compare(a) == VCRelation::Less);
    assert(a.compare(a) == VCRelation::Equal);

    // Test 2: Concurrent clocks
    b.tick(2); // b: {2:1}
    assert(a.compare(b) == VCRelation::Concurrent);
    assert(b.compare(a) == VCRelation::Concurrent);

    // Test 3: Merge and ordering
    a.merge(b); // a: {1:1,2:1}
    assert(a.compare(b) == VCRelation::Greater);
    assert(b.compare(a) == VCRelation::Less);

    // Test 4: Increment after merge
    b.tick(1); // b: {2:1,1:1}
    assert(a.compare(b) == VCRelation::Equal);
    b.tick(1); // b: {2:1,1:2}
    assert(a.compare(b) == VCRelation::Less);
    assert(b.compare(a) == VCRelation::Greater);

    // Test 5: Larger IDs and missing entries
    VectorClock c;
    c.tick(5); // {5:1}
    assert(c.compare(a) == VCRelation::Concurrent);
    a.merge(c); // a now has {1:1,2:1,5:1}
    assert(a.compare(c) == VCRelation::Greater);
    assert(c.compare(a) == VCRelation::Less);

    // Test 6: Equality after identical merges
    VectorClock d = a;
    d.merge(c);
    assert(d.compare(a) == VCRelation::Equal);

    std::cout << "All VectorClock tests passed.\n";
    return 0;
}
