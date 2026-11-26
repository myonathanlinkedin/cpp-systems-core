#include <numeric>
#include <cmath>

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cassert>
#include <algorithm>

struct Rect {
    double xmin, ymin, xmax, ymax;
    Rect() : xmin(0), ymin(0), xmax(0), ymax(0) {};
    Rect(double x1, double y1, double x2, double y2)
        : xmin(x1), ymin(y1), xmax(x2), ymax(y2) {}
    bool intersects(const Rect& o) const {
        return !(xmax < o.xmin || o.xmax < xmin ||
                 ymax < o.ymin || o.ymax < ymin);
    }
    bool contains(const Rect& o) const {
        return xmin <= o.xmin && ymin <= o.ymin &&
               xmax >= o.xmax && ymax >= o.ymax;
    }
    static Rect combine(const Rect& a, const Rect& b) {
        return Rect(std::min(a.xmin, b.xmin), std::min(a.ymin, b.ymin),
                    std::max(a.xmax, b.xmax), std::max(a.ymax, b.ymax));
    }
};

struct Node {
    bool leaf;
    std::vector<Rect> mbrs;          // bounding boxes of children or entries
    std::vector<Node*> children;     // child nodes (nullptr for leaf entries)
    Node* parent;
    Node* next;                      // next leaf node
    Node(bool l) : leaf(l), parent(nullptr), next(nullptr) {};
};

class RTree {
public:
    RTree(int maxEnt = 4) : maxEntries(maxEnt), minEntries(maxEnt / 2) {
        root = new Node(true);
    };
    ~RTree() { clear(root); }

    void insert(const Rect& r) {
        Node* leaf = chooseLeaf(root, r);
        leaf->mbrs.push_back(r);
        leaf->children.push_back(nullptr);
        adjustMBR(leaf);
        if ((int)leaf->mbrs.size() > maxEntries) splitNode(leaf);
    }

    std::vector<Rect> search(const Rect& q) const {
        std::vector<Rect> result;
        std::stack<Node*> st;
        st.push(root);
        while (!st.empty()) {
            Node* n = st.top(); st.pop();
            for (size_t i = 0; i < n->mbrs.size(); ++i) {
                if (!n->mbrs[i].intersects(q)) continue;
                if (n->leaf) {
                    if (n->mbrs[i].intersects(q))
                        result.push_back(n->mbrs[i]);
                } else {
                    st.push(n->children[i]);
                }
            }
        }
        return result;
    }

private:
    Node* root;
    int maxEntries;
    int minEntries;

    void clear(Node* n) {
        for (Node* c : n->children) if (c) clear(c);
        delete n;
    }

    Node* chooseLeaf(Node* n, const Rect& r) {
        if (n->leaf) return n;
        double minEnlargement = std::numeric_limits<double>::max();
        int best = 0;
        for (size_t i = 0; i < n->mbrs.size(); ++i) {
            Rect combined = Rect::combine(n->mbrs[i], r);
            double area = (combined.xmax - combined.xmin) *
                          (combined.ymax - combined.ymin);
            double current = (n->mbrs[i].xmax - n->mbrs[i].xmin) *
                             (n->mbrs[i].ymax - n->mbrs[i].ymin);
            double enlargement = area - current;
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                best = i;
            }
        }
        return chooseLeaf(n->children[best], r);
    }

    void adjustMBR(Node* n) {
        while (n) {
            Rect mbr;
            if (!n->mbrs.empty()) {
                mbr = n->mbrs[0];
                for (size_t i = 1; i < n->mbrs.size(); ++i)
                    mbr = Rect::combine(mbr, n->mbrs[i]);
            }
            if (n->parent) {
                size_t idx = 0;
                while (idx < n->parent->children.size() &&
                       n->parent->children[idx] != n) ++idx;
                n->parent->mbrs[idx] = mbr;
            }
            n = n->parent;
        }
    }

    void splitNode(Node* n) {
        int seed1 = 0, seed2 = 0;
        double maxDist = -1.0;
        for (size_t i = 0; i < n->mbrs.size(); ++i)
            for (size_t j = i + 1; j < n->mbrs.size(); ++j) {
                double dx = std::abs(n->mbrs[i].xmin - n->mbrs[j].xmin);
                double dy = std::abs(n->mbrs[i].ymin - n->mbrs[j].ymin);
                double dist = dx + dy;
                if (dist > maxDist) {
                    maxDist = dist;
                    seed1 = i; seed2 = j;
                }
            }
        Node* n1 = new Node(n->leaf);
        Node* n2 = new Node(n->leaf);
        n1->parent = n->parent; n2->parent = n->parent;
        n1->mbrs.push_back(n->mbrs[seed1]); n1->children.push_back(n->children[seed1]);
        n2->mbrs.push_back(n->mbrs[seed2]); n2->children.push_back(n->children[seed2]);

        std::vector<bool> assigned(n->mbrs.size(), false);
        assigned[seed1] = assigned[seed2] = true;

        for (size_t i = 0; i < n->mbrs.size(); ++i) if (!assigned[i]) {
            Rect r = n->mbrs[i];
            Rect mbr1 = n1->mbrs[0];
            for (size_t k = 1; k < n1->mbrs.size(); ++k)
                mbr1 = Rect::combine(mbr1, n1->mbrs[k]);
            Rect mbr2 = n2->mbrs[0];
            for (size_t k = 1; k < n2->mbrs.size(); ++k)
                mbr2 = Rect::combine(mbr2, n2->mbrs[k]);

            double area1 = (mbr1.xmax - mbr1.xmin) * (mbr1.ymax - mbr1.ymin);
            double area2 = (mbr2.xmax - mbr2.xmin) * (mbr2.ymax - mbr2.ymin);
            double newArea1 = (Rect::combine(mbr1, r).xmax - Rect::combine(mbr1, r).xmin) *
                              (Rect::combine(mbr1, r).ymax - Rect::combine(mbr1, r).ymin);
            double newArea2 = (Rect::combine(mbr2, r).xmax - Rect::combine(mbr2, r).xmin) *
                              (Rect::combine(mbr2, r).ymax - Rect::combine(mbr2, r).ymin);
            if (newArea1 - area1 < newArea2 - area2)
                n1->mbrs.push_back(r), n1->children.push_back(n->children[i]);
            else
                n2->mbrs.push_back(r), n2->children.push_back(n->children[i]);
        }

        if (!n->parent) {
            Node* newRoot = new Node(false);
            newRoot->mbrs.push_back(n1->mbrs[0]); newRoot->children.push_back(n1);
            newRoot->mbrs.push_back(n2->mbrs[0]); newRoot->children.push_back(n2);
            root = newRoot;
        } else {
            Node* p = n->parent;
            size_t idx = 0;
            while (idx < p->children.size() && p->children[idx] != n) ++idx;
            p->children.erase(p->children.begin() + idx);
            p->mbrs.erase(p->mbrs.begin() + idx);
            p->children.push_back(n1); p->mbrs.push_back(n1->mbrs[0]);
            p->children.push_back(n2); p->mbrs.push_back(n2->mbrs[0]);
            adjustMBR(p);
            if ((int)p->mbrs.size() > maxEntries) splitNode(p);
        }
        delete n;
    }
};
