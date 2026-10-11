#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <utility>

template<typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(size_t capacity);
    void push(const T& item);
    void push(T&& item);
    T pop();
    bool try_pop(T& out, std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    size_t size() const;
    size_t capacity() const;
private:
    std::vector<T> buffer_;
    const size_t capacity_;
    size_t head_;
    size_t tail_;
    size_t count_;
    mutable std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};
