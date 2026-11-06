#include <iostream>
#include <cassert>
#include <cstdint>
#include "core.cpp" // Pull in the implementation and declarations.

using namespace zcbd;

int main() {
    // Basic enqueue/dequeue test.
    RingBuffer buf(3);
    uint8_t a[] = {1, 2, 3};
    uint8_t b[] = {4, 5};
    uint8_t c[] = {6};

    // Enqueue three packets (fills the buffer).
    assert(buf.enqueue(a, sizeof(a)) == true);
    assert(buf.enqueue(b, sizeof(b)) == true);
    assert(buf.enqueue(c, sizeof(c)) == true);
    assert(buf.full() == true);
    assert(buf.size() == 3);

    // Attempt to enqueue when full should fail.
    uint8_t d[] = {7, 8, 9, 10};
    assert(buf.enqueue(d, sizeof(d)) == false);

    // Dequeue in FIFO order.
    const uint8_t* data = nullptr;
    std::size_t len = 0;

    assert(buf.dequeue(data, len) == true);
    assert(data == a && len == sizeof(a));

    assert(buf.dequeue(data, len) == true);
    assert(data == b && len == sizeof(b));

    assert(buf.dequeue(data, len) == true);
    assert(data == c && len == sizeof(c));

    // Buffer should be empty now.
    assert(buf.empty() == true);
    assert(buf.size() == 0);
    assert(buf.dequeue(data, len) == false); // nothing to dequeue

    // Wrap‑around test.
    assert(buf.enqueue(a, sizeof(a)) == true);
    assert(buf.enqueue(b, sizeof(b)) == true);
    assert(buf.dequeue(data, len) == true); // removes a
    assert(buf.enqueue(c, sizeof(c)) == true); // should occupy the slot freed by a
    assert(buf.full() == true);

    // Verify order after wrap‑around.
    assert(buf.dequeue(data, len) == true);
    assert(data == b && len == sizeof(b));
    assert(buf.dequeue(data, len) == true);
    assert(data == c && len == sizeof(c));
    assert(buf.empty() == true);

    // Zero‑length packet test.
    uint8_t empty_buf[1]; // dummy storage; pointer may be nullptr for zero length.
    assert(buf.enqueue(nullptr, 0) == true);
    assert(buf.full() == false);
    assert(buf.dequeue(data, len) == true);
    assert(data == nullptr && len == 0);
    assert(buf.empty() == true);

    // Stress test: fill and empty repeatedly.
    for (int i = 0; i < 1000; ++i) {
        assert(buf.enqueue(a, sizeof(a)) == true);
        assert(buf.enqueue(b, sizeof(b)) == true);
        assert(buf.full() == true);
        assert(buf.dequeue(data, len) == true);
        assert(data == a);
        assert(buf.dequeue(data, len) == true);
        assert(data == b);
        assert(buf.empty() == true);
    }

    std::cout << "All tests passed.\n";
    return 0;
}
