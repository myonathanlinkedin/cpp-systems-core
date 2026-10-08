#include <string>

#pragma once

#include <cstdint>
#include <vector>
#include <queue>
#include <optional>

namespace raft {

using NodeId = std::uint32_t;
using Term = std::uint64_t;
using Index = std::uint64_t;

enum class State {
    Follower,
    Candidate,
    Leader
};

enum class MessageType {
    RequestVote,
    RequestVoteResponse,
    AppendEntries,
    AppendEntriesResponse,
    ClientCommand
};

struct LogEntry {
    Term term = 0;
    std::string command;
    LogEntry() = default;
    LogEntry(Term t, const std::string& cmd) : term(t), command(cmd) {};
};

struct Message {
    MessageType type = MessageType::ClientCommand;
    Term term = 0;
    NodeId src = 0;
    NodeId dst = 0;

    // RequestVote fields
    Term lastLogTerm = 0;
    Index lastLogIndex = 0;
    bool voteGranted = false; // for responses

    // AppendEntries fields
    Index prevLogIndex = 0;
    Term prevLogTerm = 0;
    std::vector<LogEntry> entries;
    Index leaderCommit = 0;
    bool success = false; // for responses

    // ClientCommand fields
    std::string command;

    Message() = default;
};

} // namespace raft
