#include <vector>
#include <cassert>
#include <stack>
#include <queue>
#include <algorithm>
#include <utility>

#include "types.hpp"

namespace treearch {

// ---------------------------------------------------------------------------
// Tree constructor
// ---------------------------------------------------------------------------
Tree::Tree(int root_value) {
    // Create root node with parent = -1
    TreeNode root(root_value, static_cast<NodeId>(-1));
    nodes_.push_back(root);
    root_id_ = 0;
}

// ---------------------------------------------------------------------------
// add_child
// ---------------------------------------------------------------------------
NodeId Tree::add_child(NodeId parent_id, int child_value) {
    assert(parent_id < nodes_.size() && "Parent identifier out of bounds");
    TreeNode child(child_value, parent_id);
    nodes_.push_back(child);
    NodeId child_id = nodes_.size() - 1;
    nodes_[parent_id].children.push_back(child_id);
    return child_id;
}

// ---------------------------------------------------------------------------
// node accessor
// ---------------------------------------------------------------------------
const TreeNode& Tree::node(NodeId id) const {
    assert(id < nodes_.size() && "Node identifier out of bounds");
    return nodes_[id];
}

// ---------------------------------------------------------------------------
// depth
// ---------------------------------------------------------------------------
std::size_t Tree::depth(NodeId id) const {
    assert(id < nodes_.size() && "Node identifier out of bounds");
    std::size_t d = 0;
    NodeId cur = id;
    while (cur != static_cast<NodeId>(-1)) {
        cur = nodes_[cur].parent;
        if (cur != static_cast<NodeId>(-1))
            ++d;
    }
    return d;
}

// ---------------------------------------------------------------------------
// height (public wrapper)
// ---------------------------------------------------------------------------
std::size_t Tree::height(NodeId id) const {
    assert(id < nodes_.size() && "Node identifier out of bounds");
    return height_recursive(id);
}

// ---------------------------------------------------------------------------
// height_recursive (private)
// ---------------------------------------------------------------------------
std::size_t Tree::height_recursive(NodeId id) const {
    const auto& nd = nodes_[id];
    if (nd.children.empty())
        return 0; // leaf
    std::size_t max_child_h = 0;
    for (NodeId child_id : nd.children) {
        max_child_h = std::max(max_child_h, height_recursive(child_id));
    }
    return max_child_h + 1;
}

// ---------------------------------------------------------------------------
// preorder traversal
// ---------------------------------------------------------------------------
std::vector<int> Tree::preorder() const {
    std::vector<int> result;
    result.reserve(nodes_.size());

    std::stack<NodeId> stk;
    stk.push(root_id_);

    while (!stk.empty()) {
        NodeId cur = stk.top();
        stk.pop();
        const auto& nd = nodes_[cur];
        result.push_back(nd.value);
        // Push children in reverse order so that leftmost child is processed first
        for (auto it = nd.children.rbegin(); it != nd.children.rend(); ++it) {
            stk.push(*it);
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// postorder traversal
// ---------------------------------------------------------------------------
std::vector<int> Tree::postorder() const {
    std::vector<int> result;
    result.reserve(nodes_.size());

    std::stack<std::pair<NodeId, bool>> stk; // bool indicates whether children have been visited
    stk.emplace(root_id_, false);

    while (!stk.empty()) {
        auto [cur, visited] = stk.top();
        stk.pop();
        const auto& nd = nodes_[cur];
        if (visited) {
            result.push_back(nd.value);
        } else {
            // Postorder: children first, then node
            stk.emplace(cur, true);
            // Push children in reverse order to maintain left‑to‑right processing
            for (auto it = nd.children.rbegin(); it != nd.children.rend(); ++it) {
                stk.emplace(*it, false);
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// level-order traversal
// ---------------------------------------------------------------------------
std::vector<int> Tree::level_order() const {
    std::vector<int> result;
    result.reserve(nodes_.size());

    std::queue<NodeId> q;
    q.push(root_id_);

    while (!q.empty()) {
        NodeId cur = q.front();
        q.pop();
        const auto& nd = nodes_[cur];
        result.push_back(nd.value);
        for (NodeId child_id : nd.children) {
            q.push(child_id);
        }
    }
    return result;
}

} // namespace treearch
