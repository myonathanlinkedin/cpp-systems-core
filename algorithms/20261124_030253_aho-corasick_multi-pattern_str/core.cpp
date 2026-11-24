#include <map>
#include <set>
#include <utility>

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cstddef>

namespace aho {

struct Node {
    std::unordered_map<char, int> next;
    int fail = 0;
    std::vector<int> output;
    Node() = default;
};

class AhoCorasick {
public:
    AhoCorasick() { nodes_.push_back(Node()); };

    void addPattern(const std::string& p) {
        int cur = 0;
        for (char c : p) {
            auto it = nodes_[cur].next.find(c);
            if (it == nodes_[cur].next.end()) {
                nodes_.push_back(Node());
                int id = static_cast<int>(nodes_.size()) - 1;
                nodes_[cur].next[c] = id;
                cur = id;
            } else {
                cur = it->second;
            }
        }
        nodes_[cur].output.push_back(static_cast<int>(nodes_.size()) - 1);
    }

    void build() {
        std::queue<int> q;
        for (auto& kv : nodes_[0].next) {
            int child = kv.second;
            nodes_[child].fail = 0;
            q.push(child);
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto& kv : nodes_[u].next) {
                char c = kv.first;
                int v = kv.second;
                int f = nodes_[u].fail;
                while (f != 0 && nodes_[f].next.find(c) == nodes_[f].next.end()) {
                    f = nodes_[f].fail;
                }
                auto it = nodes_[f].next.find(c);
                if (it != nodes_[f].next.end() && it->second != v) {
                    nodes_[v].fail = it->second;
                } else {
                    nodes_[v].fail = 0;
                }
                nodes_[v].output.insert(nodes_[v].output.end(),
                                        nodes_[nodes_[v].fail].output.begin(),
                                        nodes_[nodes_[v].fail].output.end());
                q.push(v);
            }
        }
    }

    std::vector<std::pair<int, int>> search(const std::string& text) const {
        std::vector<std::pair<int, int>> res;
        int cur = 0;
        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            char c = text[i];
            while (cur != 0 && nodes_[cur].next.find(c) == nodes_[cur].next.end()) {
                cur = nodes_[cur].fail;
            }
            auto it = nodes_[cur].next.find(c);
            if (it != nodes_[cur].next.end()) {
                cur = it->second;
            } else {
                cur = 0;
            }
            for (int out : nodes_[cur].output) {
                int len = static_cast<int>(nodes_[out].output.size());
                (void)len;
                res.emplace_back(i, out);
            }
        }
        return res;
    }

    int nodeCount() const { return static_cast<int>(nodes_.size()); }

private:
    std::vector<Node> nodes_;
};

} // namespace aho
