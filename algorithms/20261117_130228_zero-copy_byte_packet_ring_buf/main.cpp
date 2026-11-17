#include <iostream>
#include <vector>
#include <cassert>
#include "core.cpp"   // Include implementation directly for this self‑contained example

void test_basic_enqueue_dequeue() {
    RingBufferDispatcher rb(4);
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {4, 5};

    assert(rb.isEmpty());
    assert(rb.enqueue(data1.data(), data1.size()));
    assert(!rb.isEmpty());
    assert(rb.size() == 1);

    auto peeked = rb.peek();
    assert(peeked.has_value());
    assert(peeked->data == data1.data());
    assert(peeked->size == data1.size());

    auto pkt = rb.dequeue();
    assert(pkt.has_value());
    assert(pkt->data == data1.data());
    assert(pkt->size == data1.size());
    assert(rb.isEmpty());

    // Enqueue second packet
    assert(rb.enqueue(data2.data(), data2.size()));
    pkt = rb.dequeue();
    assert(pkt.has_value());
    assert(pkt->data == data2.data());
    assert(pkt->size == data2.size());
    assert(rb.isEmpty());
}

void test_wrap_around() {
    RingBufferDispatcher rb(3);
    std::vector<uint8_t> a = {10};
    std::vector<uint8_t> b = {20};
    std::vector<uint8_t> c = {30};

    assert(rb.enqueue(a.data(), a.size()));
    assert(rb.enqueue(b.data(), b.size()));
    assert(rb.dequeue().has_value()); // removes a
    assert(rb.enqueue(c.data(), c.size())); // should wrap tail to index 0

    // Buffer now holds b (head) and c (tail)
    auto pkt = rb.dequeue();
    assert(pkt.has_value());
    assert(pkt->data == b.data());

    pkt = rb.dequeue();
    assert(pkt.has_value());
    assert(pkt->data == c.data());

    assert(rb.isEmpty());
}

void test_full_condition() {
    RingBufferDispatcher rb(2);
    std::vector<uint8_t> x = {1};
    std::vector<uint8_t> y = {2};
    std::vector<uint8_t> z = {3};

    assert(rb.enqueue(x.data(), x.size()));
    assert(rb.enqueue(y.data(), y.size()));
    assert(rb.isFull());
    assert(!rb.enqueue(z.data(), z.size())); // should reject

    // Dequeue one, then enqueue should succeed
    assert(rb.dequeue().has_value());
    assert(!rb.isFull());
    assert(rb.enqueue(z.data(), z.size()));
    assert(rb.isFull());
}

void test_empty_dequeue_peek() {
    RingBufferDispatcher rb(1);
    assert(rb.isEmpty());
    assert(!rb.dequeue().has_value());
    assert(!rb.peek().has_value());
}

int main() {
    test_basic_enqueue_dequeue();
    test_wrap_around();
    test_full_condition();
    test_empty_dequeue_peek();

    std::cout << "All RingBufferDispatcher tests passed.\n";
    return 0;
}
