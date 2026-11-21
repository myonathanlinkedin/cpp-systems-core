#include <vector>
#include <memory>
#include <cassert>
#include <numeric>
#include <cmath>

#include "types.hpp"

namespace stratum {

Node::Node(bool isLeaf) : leaf(isLeaf), mbr() {}

void Node::recalcMBR() {
    if (leaf) {
        assert(!entries.empty());
        mbr = entries.front().box;
        for (std::size_t i = 1; i < entries.size(); ++i) {
            mbr = combine(mbr, entries[i].box);
        }
    } else {
        assert(!childs.empty());
        mbr = childs.front()->mbr;
        for (std::size_t i = 1; i < childs.size(); ++i) {
            mbr = combine(mbr, childs[i]->mbr);
        }
    }
}

// ---------- Tree implementation ----------
Tree::Tree(std::size_t maxE) : maxEntries(maxE), minEntries(maxE / 2) {
    assert(maxEntries >= 4); // reasonable lower bound
    root = std::make_unique<Node>(true); // start with a leaf root
}

void Tree::insert(int id, const Box& box) {
    Entry e(id, box);
    insertRecursive(root.get(), e);
    if (root->leaf && root->entries.size() > maxEntries) {
        // Root overflow: split and create new root.
        auto oldRoot = std::move(root);
        root = std::make_unique<Node>(false);
        root->childs.push_back(std::move(oldRoot));
        splitNode(root.get());
    } else if (!root->leaf && root->childs.size() > maxEntries) {
        // Internal root overflow.
        splitNode(root.get());
    }
}

// Choose the leaf that needs the least enlargement to accommodate e.
void Tree::insertRecursive(Node* node, const Entry& e) {
    if (node->leaf) {
        node->entries.push_back(e);
        node->recalcMBR();
        if (node->entries.size() > maxEntries) {
            splitNode(node);
        }
        return;
    }

    // Pick child with minimal enlargement.
    Node* bestChild = nullptr;
    double bestEnlargement = std::numeric_limits<double>::infinity();
    double bestVolume = 0.0;

    for (auto& childPtr : node->childs) {
        const Box& childMBR = childPtr->mbr;
        Box enlarged = combine(childMBR, e.box);
        double enlargement = volume(enlarged) - volume(childMBR);
        if (enlargement < bestEnlargement ||
            (enlargement == bestEnlargement && volume(childMBR) < bestVolume)) {
            bestEnlargement = enlargement;
            bestVolume = volume(childMBR);
            bestChild = childPtr.get();
        }
    }
    assert(bestChild != nullptr);
    insertRecursive(bestChild, e);
    node->recalcMBR(); // after child may have split
}

// Linear split (as in original R‑tree paper).
void Tree::splitNode(Node* node) {
    // Determine seed entries/children that are farthest apart.
    // For leaf nodes we split entries; for internal nodes we split children.
    if (node->leaf) {
        // ---- Leaf split ----
        std::size_t n = node->entries.size();
        assert(n > maxEntries); // overflow guaranteed

        // Find pair with max dead space.
        std::size_t seed1 = 0, seed2 = 1;
        double maxD = -1.0;
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                Box combined = combine(node->entries[i].box, node->entries[j].box);
                double d = volume(combined) - volume(node->entries[i].box) - volume(node->entries[j].box);
                if (d > maxD) {
                    maxD = d;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }

        // Create two new nodes.
        auto nodeA = std::make_unique<Node>(true);
        auto nodeB = std::make_unique<Node>(true);
        nodeA->entries.push_back(node->entries[seed1]);
        nodeB->entries.push_back(node->entries[seed2]);

        // Mark assigned.
        std::vector<bool> assigned(n, false);
        assigned[seed1] = assigned[seed2] = true;

        // Distribute remaining entries.
        while (true) {
            // Check termination condition.
            std::size_t remaining = 0;
            for (bool a : assigned) if (!a) ++remaining;
            if (remaining == 0) break;

            // If one group would fall below minEntries, assign all remaining to it.
            if (nodeA->entries.size() + remaining == minEntries) {
                for (std::size_t i = 0; i < n; ++i) if (!assigned[i]) {
                    nodeA->entries.push_back(node->entries[i]);
                    assigned[i] = true;
                }
                break;
            }
            if (nodeB->entries.size() + remaining == minEntries) {
                for (std::size_t i = 0; i < n; ++i) if (!assigned[i]) {
                    nodeB->entries.push_back(node->entries[i]);
                    assigned[i] = true;
                }
                break;
            }

            // Choose next entry with greatest preference.
            double bestDiff = -1.0;
            std::size_t bestIdx = n; // sentinel
            bool assignToA = true;

            Box mbrA = nodeA->entries.empty() ? node->entries[seed1].box : nodeA->mbr;
            Box mbrB = nodeB->entries.empty() ? node->entries[seed2].box : nodeB->mbr;

            for (std::size_t i = 0; i < n; ++i) {
                if (assigned[i]) continue;
                const Box& curBox = node->entries[i].box;
                double incA = volume(combine(mbrA, curBox)) - volume(mbrA);
                double incB = volume(combine(mbrB, curBox)) - volume(mbrB);
                double diff = std::abs(incA - incB);
                if (diff > bestDiff) {
                    bestDiff = diff;
                    bestIdx = i;
                    assignToA = incA < incB;
                }
            }
            assert(bestIdx < n);
            if (assignToA) nodeA->entries.push_back(node->entries[bestIdx]);
            else          nodeB->entries.push_back(node->entries[bestIdx]);
            assigned[bestIdx] = true;
        }

        nodeA->recalcMBR();
        nodeB->recalcMBR();

        // Replace contents of original node with nodeA and push nodeB as sibling.
        node->leaf = false;
        node->entries.clear();
        node->childs.clear();
        node->childs.push_back(std::move(nodeA));
        node->childs.push_back(std::move(nodeB));
        node->recalcMBR();
    } else {
        // ---- Internal node split ----
        std::size_t n = node->childs.size();
        assert(n > maxEntries);

        // Find seed children farthest apart.
        std::size_t seed1 = 0, seed2 = 1;
        double maxD = -1.0;
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                Box combined = combine(node->childs[i]->mbr, node->childs[j]->mbr);
                double d = volume(combined) - volume(node->childs[i]->mbr) - volume(node->childs[j]->mbr);
                if (d > maxD) {
                    maxD = d;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }

        auto nodeA = std::make_unique<Node>(false);
        auto nodeB = std::make_unique<Node>(false);
        nodeA->childs.push_back(std::move(node->childs[seed1]));
        nodeB->childs.push_back(std::move(node->childs[seed2]));

        std::vector<bool> assigned(n, false);
        assigned[seed1] = assigned[seed2] = true;

        while (true) {
            std::size_t remaining = 0;
            for (bool a : assigned) if (!a) ++remaining;
            if (remaining == 0) break;

            if (nodeA->childs.size() + remaining == minEntries) {
                for (std::size_t i = 0; i < n; ++i) if (!assigned[i]) {
                    nodeA->childs.push_back(std::move(node->childs[i]));
                    assigned[i] = true;
                }
                break;
            }
            if (nodeB->childs.size() + remaining == minEntries) {
                for (std::size_t i = 0; i < n; ++i) if (!assigned[i]) {
                    nodeB->childs.push_back(std::move(node->childs[i]));
                    assigned[i] = true;
                }
                break;
            }

            double bestDiff = -1.0;
            std::size_t bestIdx = n;
            bool assignToA = true;

            Box mbrA = nodeA->childs.empty() ? node->childs[seed1]->mbr : nodeA->mbr;
            Box mbrB = nodeB->childs.empty() ? node->childs[seed2]->mbr : nodeB->mbr;

            for (std::size_t i = 0; i < n; ++i) {
                if (assigned[i]) continue;
                const Box& curBox = node->childs[i]->mbr;
                double incA = volume(combine(mbrA, curBox)) - volume(mbrA);
                double incB = volume(combine(mbrB, curBox)) - volume(mbrB);
                double diff = std::abs(incA - incB);
                if (diff > bestDiff) {
                    bestDiff = diff;
                    bestIdx = i;
                    assignToA = incA < incB;
                }
            }
            assert(bestIdx < n);
            if (assignToA) nodeA->childs.push_back(std::move(node->childs[bestIdx]));
            else          nodeB->childs.push_back(std::move(node->childs[bestIdx]));
            assigned[bestIdx] = true;
        }

        nodeA->recalcMBR();
        nodeB->recalcMBR();

        // Replace original node with nodeA and add nodeB as sibling.
        node->leaf = false;
        node->entries.clear();
        node->childs.clear();
        node->childs.push_back(std::move(nodeA));
        node->childs.push_back(std::move(nodeB));
        node->recalcMBR();
    }
}

// ---------- Query ----------
std::vector<int> Tree::query(const Box& q) const {
    std::vector<int> result;
    queryRecursive(root.get(), q, result);
    return result;
}

void Tree::queryRecursive(const Node* node, const Box& q, std::vector<int>& out) const {
    if (!intersects(node->mbr, q)) return; // prune

    if (node->leaf) {
        for (const auto& e : node->entries) {
            if (intersects(e.box, q)) out.push_back(e.id);
        }
    } else {
        for (const auto& child : node->childs) {
            queryRecursive(child.get(), q, out);
        }
    }
}

} // namespace stratum
