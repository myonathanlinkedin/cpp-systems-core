#include "core.hpp"

BoundedBlockingQueue::BoundedBlockingQueue(std::size_t capacity) : capacity_(capacity) {}

void BoundedBlockingQueue::push(const QueueElement& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_full_.wait(lock, [this] { return buffer_.size() < capacity_; });
    buffer_.push_back(value);
    not_empty_.notify_one();
}

QueueElement BoundedBlockingQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_.wait(lock, [this] { return !buffer_.empty(); });
    QueueElement val = buffer_.front();
    buffer_.pop_front();
    not_full_.notify_one();
    return val;
}

std::size_t BoundedBlockingQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

bool BoundedBlockingQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
}

bool BoundedBlockingQueue::full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size() >= capacity_;
}
