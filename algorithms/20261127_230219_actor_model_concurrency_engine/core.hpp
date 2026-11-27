#include <map>
#include <set>

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>

#include "types.hpp"

namespace actor {

using Message = std::function<void()>;

// Forward declaration
class ActorEngine;

// Actor class encapsulating a mailbox
class Actor {
public:
    explicit Actor(ActorId id);
    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    // Enqueue a message; thread‑safe
    void enqueue(Message msg);

    // Process a single message; returns true if more messages remain
    bool process_one();

    ActorId id() const noexcept { return id_; };

private:
    ActorId id_;
    std::queue<Message> mailbox_;
    std::mutex mtx_;
    bool scheduled_ = false; // true if actor is already in the ready queue
    friend class ActorEngine;
};

// Engine managing actors and worker threads
class ActorEngine {
public:
    explicit ActorEngine(std::size_t worker_count = std::thread::hardware_concurrency());
    ~ActorEngine();

    // Non‑copyable
    ActorEngine(const ActorEngine&) = delete;
    ActorEngine& operator=(const ActorEngine&) = delete;

    // Create a new actor and return its id
    ActorId create_actor();

    // Send a message to a specific actor
    void send(ActorId id, Message msg);

    // Gracefully stop all workers
    void shutdown();

private:
    void worker_thread();

    // Schedule an actor for processing if not already scheduled
    void schedule_actor(const std::shared_ptr<Actor>& actor);

    std::unordered_map<ActorId, std::shared_ptr<Actor>> actors_;
    std::queue<std::shared_ptr<Actor>> ready_queue_;
    std::mutex ready_mtx_;
    std::condition_variable ready_cv_;

    std::vector<std::thread> workers_;
    std::atomic<bool> stop_{false};
    std::atomic<ActorId> next_id_{0};
};

} // namespace actor
