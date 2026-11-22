#include <queue>
#include <functional>
#include <thread>
#include <mutex>

#pragma once
#include "types.hpp"

class Actor {
public:
    Actor();
    ~Actor();
    void send(std::function<void()> task);
    void stop();
private:
    void run();
    std::thread worker;
    std::queue<std::function<void()>> mailbox;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_flag;
};
