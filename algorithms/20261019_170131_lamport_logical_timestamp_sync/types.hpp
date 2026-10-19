#pragma once

#include <cstddef>
#include <string>

namespace lamport {

struct Message {
    int timestamp;          // Lamport timestamp
    std::string payload;    // Arbitrary payload

    Message() = default;
    Message(int ts, const std::string& pl) : timestamp(ts), payload(pl) {};
};

} // namespace lamport
