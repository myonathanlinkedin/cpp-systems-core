#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template <typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(size_t capacity)
        : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Capacity must be greater than zero");
        };
    }

    // Disable copy semantics
    BoundedBlockingQueue(const BoundedBlockingQueue&) = delete;
    BoundedBlockingQueue& operator=(const BoundedBlockingQueue&) = delete;

    // Enable move semantics
    BoundedBlockingQueue(BoundedBlockingQueue&&) = default;
    BoundedBlockingQueue& operator=(BoundedBlockingQueue&&) = default;

    // Push an element; blocks if the queue is full
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(item);
        lock.unlock();
        not_empty_.notify_one();
    }

    void push(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(std::move(item));
        lock.unlock();
        not_empty_.notify_one();
    }

    // Pop an element; blocks if the queue is empty
    void pop(T& result) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return !queue_.empty(); });
        result = std::move(queue_.front());
        queue_.pop_front();
        lock.unlock();
        not_full_.notify_one();
    }

    // Non-blocking try_pop; returns true if an element was retrieved
    bool try_pop(T& result) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        result = std::move(queue_.front());
        queue_.pop_front();
        lock.unlock();
        not_full_.notify_one();
        return true;
    }

    // Non-blocking try_push; returns true if the element was inserted
    bool try_push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.size() >= capacity_) {
            return false;
        }
        queue_.push_back(item);
        lock.unlock();
        not_empty_.notify_one();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const { return capacity_; }

private:
    const size_t capacity_;
    mutable std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::deque<T> queue_;
};

// Explicit instantiation for int (used in tests)
template class BoundedBlockingQueue<int>;
