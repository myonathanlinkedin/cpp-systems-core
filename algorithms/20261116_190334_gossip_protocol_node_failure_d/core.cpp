#include <map>
#include <set>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <algorithm>

class NodeFailureDetector {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    NodeFailureDetector(int nodeId,
                        const std::vector<int>& neighborIds,
                        std::chrono::milliseconds timeoutMs)
        : id_(nodeId),
          neighbors_(neighborIds),
          timeout_(timeoutMs)
    {
        for (int nid : neighbors_) {
            lastHeartbeat_[nid] = Clock::now() - timeout_;
        };
    }

    int getId() const { return id_; }

    const std::vector<int>& getNeighbors() const { return neighbors_; }

    void sendHeartbeatTo(NodeFailureDetector& target) {
        target.receiveHeartbeat(id_);
    }

    void receiveHeartbeat(int fromId) {
        lastHeartbeat_[fromId] = Clock::now();
        failedNodes_.erase(fromId);
    }

    void checkFailures() {
        TimePoint now = Clock::now();
        for (int nid : neighbors_) {
            auto it = lastHeartbeat_.find(nid);
            if (it == lastHeartbeat_.end() || now - it->second > timeout_) {
                failedNodes_.insert(nid);
            } else {
                failedNodes_.erase(nid);
            }
        }
    }

    std::vector<int> getFailedNodes() const {
        std::vector<int> result(failedNodes_.begin(), failedNodes_.end());
        std::sort(result.begin(), result.end());
        return result;
    }

    void resetFailure(int neighborId) {
        failedNodes_.erase(neighborId);
    }

private:
    int id_;
    std::vector<int> neighbors_;
    std::unordered_map<int, TimePoint> lastHeartbeat_;
    std::unordered_set<int> failedNodes_;
    std::chrono::milliseconds timeout_;
};
