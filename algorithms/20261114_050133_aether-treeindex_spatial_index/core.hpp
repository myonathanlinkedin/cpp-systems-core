#include <numeric>

#pragma once
#include "types.hpp"
#include <vector>
#include <memory>
#include <stack>
#include <queue>
#include <algorithm>
#include <cassert>
#include <limits>
#include <cmath>

namespace aether {

template <size_t Dim, size_t MaxNodeCapacity = 8>
class AetherTree {
public:
    struct Node;
    using NodePtr = std::unique_ptr<Node>;

    struct Node {
        bool leaf;
        BoundingBox<Dim> mbr;
        std::vector<Entry<Dim>> entries; // leaf only
        std::vector<NodePtr> children;   // internal only

        explicit Node(bool leaf_) : leaf(leaf_) {};
    };

    AetherTree() = default;

    void insert(const BoundingBox<Dim>& box, size_t id) {
        Entry<Dim> e(box, id);
        if (!root_) root_ = std::make_unique<Node>(true);
        insertRecursive(root_.get(), e);
        if (root_->leaf && root_->entries.size() > MaxNodeCapacity) splitRoot();
        else if (!root_->leaf && root_->children.size() > MaxNodeCapacity) splitRoot();
    }

    std::vector<size_t> query(const BoundingBox<Dim>& qbox) const {
        std::vector<size_t> result;
        if (!root_) return result;
        std::stack<const Node*> stk;
        stk.push(root_.get());
        while (!stk.empty()) {
            const Node* node = stk.top();
            stk.pop();
            if (!node->mbr.intersects(qbox)) continue;
            if (node->leaf) {
                for (const auto& e : node->entries)
                    if (e.box.intersects(qbox)) result.push_back(e.id);
            } else {
                for (const auto& child : node->children) stk.push(child.get());
            }
        }
        return result;
    }

private:
    NodePtr root_;
    std::stack<NodePtr> pendingSplits_;

    void insertRecursive(Node* node, const Entry<Dim>& e) {
        node->mbr.expand(e.box);
        if (node->leaf) {
            node->entries.push_back(e);
        } else {
            Node* best = nullptr;
            double bestEnlargement = std::numeric_limits<double>::infinity();
            for (auto& child : node->children) {
                double before = child->mbr.volume();
                BoundingBox<Dim> tmp = child->mbr;
                tmp.expand(e.box);
                double after = tmp.volume();
                double enlargement = after - before;
                if (enlargement < bestEnlargement) {
                    bestEnlargement = enlargement;
                    best = child.get();
                }
            }
            assert(best != nullptr);
            insertRecursive(best, e);
        }

        if (node->leaf && node->entries.size() > MaxNodeCapacity) splitNode(node);
        else if (!node->leaf && node->children.size() > MaxNodeCapacity) splitNode(node);
    }

    void splitNode(Node* node) {
        // Linear split: pick two farthest items as seeds
        NodePtr n1 = std::make_unique<Node>(node->leaf);
        NodePtr n2 = std::make_unique<Node>(node->leaf);

        if (node->leaf) {
            size_t seed1 = 0, seed2 = 0;
            double maxDist = -1.0;
            for (size_t i = 0; i < node->entries.size(); ++i) {
                for (size_t j = i + 1; j < node->entries.size(); ++j) {
                    double d = boxDistance(node->entries[i].box, node->entries[j].box);
                    if (d > maxDist) { maxDist = d; seed1 = i; seed2 = j; }
                }
            }
            n1->entries.push_back(node->entries[seed1]);
            n2->entries.push_back(node->entries[seed2]);
            for (size_t i = 0; i < node->entries.size(); ++i) {
                if (i == seed1 || i == seed2) continue;
                double inc1 = enlargement(n1->mbr, node->entries[i].box);
                double inc2 = enlargement(n2->mbr, node->entries[i].box);
                if (inc1 < inc2) n1->entries.push_back(node->entries[i]);
                else n2->entries.push_back(node->entries[i]);
            }
        } else {
            size_t seed1 = 0, seed2 = 0;
            double maxDist = -1.0;
            for (size_t i = 0; i < node->children.size(); ++i) {
                for (size_t j = i + 1; j < node->children.size(); ++j) {
                    double d = boxDistance(node->children[i]->mbr, node->children[j]->mbr);
                    if (d > maxDist) { maxDist = d; seed1 = i; seed2 = j; }
                }
            }
            n1->children.push_back(std::move(node->children[seed1]));
            n2->children.push_back(std::move(node->children[seed2]));
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (i == seed1 || i == seed2) continue;
                double inc1 = enlargement(n1->mbr, node->children[i]->mbr);
                double inc2 = enlargement(n2->mbr, node->children[i]->mbr);
                if (inc1 < inc2) n1->children.push_back(std::move(node->children[i]));
                else n2->children.push_back(std::move(node->children[i]));
            }
        }

        recomputeMBR(*n1);
        recomputeMBR(*n2);

        // Replace node with n1 and push n2 as pending sibling
        node->leaf = n1->leaf;
        node->entries = std::move(n1->entries);
        node->children = std::move(n1->children);
        node->mbr = n1->mbr;
        pendingSplits_.push(std::move(n2));
    }

    void splitRoot() {
        NodePtr oldRoot = std::move(root_);
        root_ = std::make_unique<Node>(false);
        root_->children.push_back(std::move(oldRoot));
        splitNode(root_->children[0].get());
        while (!pendingSplits_.empty()) {
            root_->children.push_back(std::move(pendingSplits_.top()));
            pendingSplits_.pop();
        }
        recomputeMBR(*root_);
    }

    static double boxDistance(const BoundingBox<Dim>& a, const BoundingBox<Dim>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < Dim; ++i) {
            double d = 0.0;
            if (a.max[i] < b.min[i]) d = b.min[i] - a.max[i];
            else if (b.max[i] < a.min[i]) d = a.min[i] - b.max[i];
            sum += d * d;
        }
        return std::sqrt(sum);
    }

    static double enlargement(const BoundingBox<Dim>& mbr, const BoundingBox<Dim>& add) {
        BoundingBox<Dim> tmp = mbr;
        tmp.expand(add);
        return tmp.volume() - mbr.volume();
    }

    static void recomputeMBR(Node& node) {
        node.mbr = BoundingBox<Dim>();
        if (node.leaf) {
            for (const auto& e : node.entries) node.mbr.expand(e.box);
        } else {
            for (const auto& child : node.children) node.mbr.expand(child->mbr);
        }
    }
};

} // namespace aether
