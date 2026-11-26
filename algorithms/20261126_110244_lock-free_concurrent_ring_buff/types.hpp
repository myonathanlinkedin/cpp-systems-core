#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace lockfree {

// Forward declaration of the ring buffer template.
template <typename T>
class RingBuffer;

// Helper struct used in unit tests.
struct TestData {
    int id;
    int value;
};

} // namespace lockfree
