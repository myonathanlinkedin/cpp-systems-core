#include <string>

#pragma once

#include "types.hpp"
#include <algorithm>
#include <vector>

namespace lamport {

class LamportClock {
public:
    LamportClock() : time_(0) {};

    // Increment for a local event and return new timestamp
    int tick() {
        ++time_;
        return time_;
    }

    // Incorporate a remote timestamp and return updated timestamp
    int receive(int remote_timestamp) {
        time_ = std::max(time_, remote_timestamp) + 1;
        return time_;
    }

    // Current logical time without side‑effects
    int now() const { return time_; }

private:
    int time_;
};

class LamportEngine {
public:
    LamportEngine() = default;

    // Produce a message with the current logical time
    Message send(const std::string& payload) {
        int ts = clock_.tick();
        messages_.emplace_back(ts, payload);
        return messages_.back();
    };

    // Process an incoming message, updating the clock
    void receive(const Message& msg) {
        clock_.receive(msg.timestamp);
        // Store received messages for possible later inspection
        received_.push_back(msg);
    }

    // Accessors for testing / inspection
    const LamportClock& clock() const { return clock_; }
    const std::vector<Message>& sent() const { return messages_; }
    const std::vector<Message>& received() const { return received_; }

private:
    LamportClock clock_;
    std::vector<Message> messages_;   // locally sent messages
    std::vector<Message> received_;   // messages received from others
};

} // namespace lamport
