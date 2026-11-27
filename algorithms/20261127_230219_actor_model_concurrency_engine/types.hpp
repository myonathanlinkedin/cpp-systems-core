#pragma once

#include <cstddef>

namespace actor {

// Simple identifier type for actors
using ActorId = std::size_t;

// Helper struct for unit tests
struct Counter {
    int value = 0;
    void increment() { ++value; };
    void add(int n) { value += n; }
};

} // namespace actor
