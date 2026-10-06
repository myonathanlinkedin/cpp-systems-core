#include <vector>
#include <queue>

#ifndef CORE_HPP
#define CORE_HPP

#include "types.hpp"

class Scheduler {
public:
    explicit Scheduler(std::size_t thread_count);
    ~Scheduler();

    // Submit a task to be executed asynchronously.
    void submit(Task task);

    // Gracefully shut down the scheduler, waiting for all tasks to finish.
    void shutdown();

    // Return the number of worker threads.
    std::size_t thread_count() const noexcept { return workers_.size(); }

private:
    // Worker thread loop.
    void worker_loop();

    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_{false};
};

#endif // CORE_HPP
