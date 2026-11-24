#pragma once
#include <string>

struct Node {
    std::string id;
    Node() = default;
    explicit Node(const std::string& id_) : id(id_) {};
};
