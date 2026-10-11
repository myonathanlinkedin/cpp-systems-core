#include <cassert>
#include <chrono>
#include <mutex>

#include "types.hpp"
#include <string>

template<typename T>
BoundedBlockingQueue<T>::BoundedBlockingQueue(size_t capacity)
    : buffer_(capacity), capacity_(capacity), head_(0), tail_(0), count_(0) {
    assert(capacity_ > 0 && "Capacity must be greater than zero");
}

template<typename T>
void BoundedBlockingQueue<T>::push(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_full_.wait(lock, [this] { return count_ < capacity_; });
    buffer_[tail_] = item;
    tail_ = (tail_ + 1) % capacity_;
    ++count_;
    lock.unlock();
    not_empty_.notify_one();
}

template<typename T>
void BoundedBlockingQueue<T>::push(T&& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_full_.wait(lock, [this] { return count_ < capacity_; });
    buffer_[tail_] = std::move(item);
    tail_ = (tail_ + 1) % capacity_;
    ++count_;
    lock.unlock();
    not_empty_.notify_one();
}

template<typename T>
T BoundedBlockingQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_.wait(lock, [this] { return count_ > 0; });
    T item = std::move(buffer_[head_]);
    head_ = (head_ + 1) % capacity_;
    --count_;
    lock.unlock();
    not_full_.notify_one();
    return item;
}

template<typename T>
bool BoundedBlockingQueue<T>::try_pop(T& out, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!not_empty_.wait_for(lock, timeout, [this] { return count_ > 0; })) {
        return false;
    }
    out = std::move(buffer_[head_]);
    head_ = (head_ + 1) % capacity_;
    --count_;
    lock.unlock();
    not_full_.notify_one();
    return true;
}

template<typename T>
size_t BoundedBlockingQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

template<typename T>
size_t BoundedBlockingQueue<T>::capacity() const {
    return capacity_;
}

// Explicit template instantiation for common types
template class BoundedBlockingQueue<int>;
template class BoundedBlockingQueue<std::string>;
