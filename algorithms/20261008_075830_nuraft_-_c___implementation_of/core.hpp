#include <string>
#include <map>
#include <set>

#pragma once

#include "types.hpp"
#include <vector>
#include <queue>
#include <unordered_map>

namespace raft {

class RaftNode {
public:
    RaftNode(NodeId id, const std::vector<NodeId>& peers);

    // Public interface
    void tick(); // advance logical clock, trigger elections if needed
    void receive(const Message& msg); // inbound message
    const std::queue<Message>& outbox() const; // messages to be sent
    void clearOutbox(); // after delivering messages
    State state() const;
    Term currentTerm() const;
    NodeId id() const;
    const std::vector<LogEntry>& log() const;
    Index commitIndex() const;

    // For test harness: inject client command
    void clientCommand(const std::string& cmd);

private:
    // Core state
    NodeId m_id;
    std::vector<NodeId> m_peers;
    State m_state = State::Follower;
    Term m_currentTerm = 0;
    std::optional<NodeId> m_votedFor;
    std::vector<LogEntry> m_log; // 1-indexed: log[0] is dummy
    Index m_commitIndex = 0;
    Index m_lastApplied = 0;

    // Leader state
    std::unordered_map<NodeId, Index> m_nextIndex;
    std::unordered_map<NodeId, Index> m_matchIndex;

    // Timing
    int m_electionTimeout = 0;
    int m_heartbeatTimeout = 0;
    const int kElectionTimeoutMin = 5;
    const int kElectionTimeoutMax = 10;
    const int kHeartbeatInterval = 2;

    // Message handling
    std::queue<Message> m_outbox;

    // Helpers
    void becomeFollower(Term term);
    void becomeCandidate();
    void becomeLeader();
    void resetElectionTimer();
    void resetHeartbeatTimer();
    void sendMessage(const Message& msg);
    void broadcast(const Message& msg);
    void handleRequestVote(const Message& msg);
    void handleRequestVoteResponse(const Message& msg);
    void handleAppendEntries(const Message& msg);
    void handleAppendEntriesResponse(const Message& msg);
    void applyLogEntries();
    Index lastLogIndex() const;
    Term lastLogTerm() const;
};

} // namespace raft
