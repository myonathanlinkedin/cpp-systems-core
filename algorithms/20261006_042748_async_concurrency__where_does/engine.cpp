#include <cassert>

#include "core.hpp"

Scheduler::Scheduler(std::size_t thread_count) {
    assert(thread_count > 0 && "Scheduler requires at least one thread");
    workers_.reserve(thread_count);
    for (std::size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back(&Scheduler::worker_loop, this);
    }
}

Scheduler::~Scheduler() {
    shutdown();
}

void Scheduler::submit(Task task) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (stop_) {
            throw std::runtime_error("Cannot submit task to stopped Scheduler");
        }
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
}

void Scheduler::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (stop_) return; // Already stopped
        stop_ = true;
    }
    cv_.notify_all();
    for (std::thread &worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
}

void Scheduler::worker_loop() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
            if (stop_ && tasks_.empty()) {
                return;
            }
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        // Execute the task outside the lock to allow other workers to proceed.
        task();
    }
}
