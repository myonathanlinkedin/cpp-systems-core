#include <memory>
#include <mutex>
#include <stdexcept>

#include "core.hpp"

namespace actor {

// ---------- Actor implementation ----------
Actor::Actor(ActorId id) : id_(id) {}

void Actor::enqueue(Message msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    mailbox_.push(std::move(msg));
}

bool Actor::process_one() {
    Message task;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (mailbox_.empty()) {
            scheduled_ = false;
            return false;
        }
        task = std::move(mailbox_.front());
        mailbox_.pop();
    }
    // Execute outside lock to avoid deadlocks
    task();
    return true;
}

// ---------- ActorEngine implementation ----------
ActorEngine::ActorEngine(std::size_t worker_count) {
    if (worker_count == 0) worker_count = 1;
    for (std::size_t i = 0; i < worker_count; ++i) {
        workers_.emplace_back(&ActorEngine::worker_thread, this);
    }
}

ActorEngine::~ActorEngine() {
    shutdown();
}

ActorId ActorEngine::create_actor() {
    ActorId id = next_id_++;
    auto actor = std::make_shared<Actor>(id);
    {
        std::lock_guard<std::mutex> lock(ready_mtx_);
        actors_.emplace(id, actor);
    }
    return id;
}

void ActorEngine::send(ActorId id, Message msg) {
    std::shared_ptr<Actor> actor;
    {
        std::lock_guard<std::mutex> lock(ready_mtx_);
        auto it = actors_.find(id);
        if (it == actors_.end()) {
            throw std::runtime_error("Actor not found");
        }
        actor = it->second;
    }
    {
        std::lock_guard<std::mutex> lock(actor->mtx_);
        actor->mailbox_.push(std::move(msg));
        if (!actor->scheduled_) {
            actor->scheduled_ = true;
            std::lock_guard<std::mutex> rq_lock(ready_mtx_);
            ready_queue_.push(actor);
            ready_cv_.notify_one();
        }
    }
}

void ActorEngine::schedule_actor(const std::shared_ptr<Actor>& actor) {
    std::lock_guard<std::mutex> lock(ready_mtx_);
    if (!actor->scheduled_) {
        actor->scheduled_ = true;
        ready_queue_.push(actor);
        ready_cv_.notify_one();
    }
}

void ActorEngine::worker_thread() {
    while (!stop_) {
        std::shared_ptr<Actor> actor;
        {
            std::unique_lock<std::mutex> lock(ready_mtx_);
            ready_cv_.wait(lock, [this] { return stop_ || !ready_queue_.empty(); });
            if (stop_ && ready_queue_.empty()) return;
            actor = ready_queue_.front();
            ready_queue_.pop();
        }
        // Process a single message
        bool more = actor->process_one();
        if (more) {
            // Re‑schedule for remaining messages
            schedule_actor(actor);
        }
    }
}

void ActorEngine::shutdown() {
    stop_ = true;
    ready_cv_.notify_all();
    for (auto& th : workers_) {
        if (th.joinable()) th.join();
    }
    // Clear remaining actors
    {
        std::lock_guard<std::mutex> lock(ready_mtx_);
        actors_.clear();
        while (!ready_queue_.empty()) ready_queue_.pop();
    }
}

} // namespace actor
