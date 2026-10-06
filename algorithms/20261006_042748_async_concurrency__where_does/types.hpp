#ifndef TYPES_HPP
#define TYPES_HPP

#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include <memory>
#include <cassert>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

// Domain model: a generic task represented as a callable with no arguments and no return value.
using Task = std::function<void()>;

// Simple thread-safe counter for demonstration purposes.
struct Counter {
    std::atomic<int> value{0};
    void increment() { ++value; }
};

#endif // TYPES_HPP
