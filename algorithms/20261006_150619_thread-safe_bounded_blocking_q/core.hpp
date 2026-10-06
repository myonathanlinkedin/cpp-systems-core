#pragma once
#include <mutex>
#include <condition_variable>
#include <deque>
#include <cstddef>
#include "types.hpp"

class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(std::size_t capacity);
    void push(const QueueElement& value);
    QueueElement pop();
    std::size_t size() const;
    bool empty() const;
    bool full() const;
private:
    std::size_t capacity_;
    std::deque<QueueElement> buffer_;
    mutable std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};
