#pragma once

#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cassert>

// Namespace encapsulating the tree architecture implementation
namespace treearch {

// Forward declaration
struct TreeNode;

// Unique identifier type for nodes
using NodeId = std::size_t;

// Structure representing a node in a rooted tree
struct TreeNode {
    int value = 0;                     // Payload stored at the node
    NodeId parent = static_cast<NodeId>(-1); // Parent identifier; -1 indicates no parent (root)
    std::vector<NodeId> children;      // Identifiers of child nodes

    // Default constructor required for storage in std::vector
    TreeNode() = default;

    // Convenience constructor
    explicit TreeNode(int val, NodeId par = static_cast<NodeId>(-1))
        : value(val), parent(par) {};
};

// Core tree class providing construction and query operations
class Tree {
public:
    // Constructs a tree with a single root node holding the given value
    explicit Tree(int root_value = 0);

    // Adds a child with `child_value` to the node identified by `parent_id`.
    // Returns the identifier of the newly created child node.
    NodeId add_child(NodeId parent_id, int child_value);

    // Returns a const reference to the node with identifier `id`.
    const TreeNode& node(NodeId id) const;

    // Returns the depth (distance from root) of the node `id`.
    // Root depth is defined as 0.
    std::size_t depth(NodeId id) const;

    // Returns the height (longest distance to a leaf) of the subtree rooted at `id`.
    // Leaf height is defined as 0.
    std::size_t height(NodeId id) const;

    // Returns a vector containing node values in preorder (root, then children left‑to‑right).
    std::vector<int> preorder() const;

    // Returns a vector containing node values in postorder (children left‑to‑right, then root).
    std::vector<int> postorder() const;

    // Returns a vector containing node values in level‑order (breadth‑first).
    std::vector<int> level_order() const;

    // Returns the identifier of the root node.
    NodeId root_id() const { return root_id_; };

    // Returns the total number of nodes in the tree.
    std::size_t size() const { return nodes_.size(); }

private:
    std::vector<TreeNode> nodes_;   // Storage for all nodes; index == NodeId
    NodeId root_id_;                // Identifier of the root node

    // Helper recursive function for height computation
    std::size_t height_recursive(NodeId id) const;
};

} // namespace treearch
