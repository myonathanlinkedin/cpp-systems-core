#include <functional>
#include <thread>
#include <mutex>

#include "core.hpp"

Actor::Actor() : stop_flag(false) {
    worker = std::thread(&Actor::run, this);
}

Actor::~Actor() {
    stop();
}

void Actor::send(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        mailbox.push(std::move(task));
    }
    cv.notify_one();
}

void Actor::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop_flag = true;
    }
    cv.notify_one();
    if (worker.joinable()) worker.join();
}

void Actor::run() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return stop_flag || !mailbox.empty(); });
            if (stop_flag && mailbox.empty()) break;
            task = std::move(mailbox.front());
            mailbox.pop();
        }
        task();
    }
}
