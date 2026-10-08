#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <map>
#include <set>

#include "core.hpp"
#include <random>
#include <algorithm>
#include <cassert>

namespace raft {

static std::random_device rd;
static std::mt19937 gen(rd());

RaftNode::RaftNode(NodeId id, const std::vector<NodeId>& peers)
    : m_id(id), m_peers(peers) {
    // dummy entry at index 0 to simplify indexing
    m_log.emplace_back(0, "");
    resetElectionTimer();
    resetHeartbeatTimer();
}

void RaftNode::tick() {
    if (m_state != State::Leader) {
        if (--m_electionTimeout <= 0) {
            becomeCandidate();
            resetElectionTimer();
        }
    } else {
        if (--m_heartbeatTimeout <= 0) {
            // send heartbeats
            Message hb;
            hb.type = MessageType::AppendEntries;
            hb.term = m_currentTerm;
            hb.src = m_id;
            hb.prevLogIndex = lastLogIndex();
            hb.prevLogTerm = lastLogTerm();
            hb.leaderCommit = m_commitIndex;
            broadcast(hb);
            resetHeartbeatTimer();
        }
    }
}

void RaftNode::receive(const Message& msg) {
    // Discard messages from unknown nodes
    if (msg.src == m_id) return;
    // Update term if needed
    if (msg.term > m_currentTerm) {
        becomeFollower(msg.term);
    }

    switch (msg.type) {
        case MessageType::RequestVote:
            handleRequestVote(msg);
            break;
        case MessageType::RequestVoteResponse:
            handleRequestVoteResponse(msg);
            break;
        case MessageType::AppendEntries:
            handleAppendEntries(msg);
            break;
        case MessageType::AppendEntriesResponse:
            handleAppendEntriesResponse(msg);
            break;
        case MessageType::ClientCommand:
            // only leader should receive client commands
            if (m_state == State::Leader) {
                clientCommand(msg.command);
            }
            break;
    }
}

const std::queue<Message>& RaftNode::outbox() const {
    return m_outbox;
}

void RaftNode::clearOutbox() {
    std::queue<Message> empty;
    std::swap(m_outbox, empty);
}

State RaftNode::state() const { return m_state; }
Term RaftNode::currentTerm() const { return m_currentTerm; }
NodeId RaftNode::id() const { return m_id; }
const std::vector<LogEntry>& RaftNode::log() const { return m_log; }
Index RaftNode::commitIndex() const { return m_commitIndex; }

void RaftNode::clientCommand(const std::string& cmd) {
    // Append to own log
    m_log.emplace_back(m_currentTerm, cmd);
    // Update nextIndex/matchIndex for followers
    for (NodeId peer : m_peers) {
        if (peer == m_id) continue;
        Message ae;
        ae.type = MessageType::AppendEntries;
        ae.term = m_currentTerm;
        ae.src = m_id;
        ae.dst = peer;
        ae.prevLogIndex = m_nextIndex[peer] - 1;
        ae.prevLogTerm = (ae.prevLogIndex < m_log.size()) ? m_log[ae.prevLogIndex].term : 0;
        // send all entries from nextIndex onward
        for (Index i = m_nextIndex[peer]; i < m_log.size(); ++i) {
            ae.entries.push_back(m_log[i]);
        }
        ae.leaderCommit = m_commitIndex;
        sendMessage(ae);
    }
}

void RaftNode::becomeFollower(Term term) {
    m_state = State::Follower;
    m_currentTerm = term;
    m_votedFor.reset();
    resetElectionTimer();
    resetHeartbeatTimer();
}

void RaftNode::becomeCandidate() {
    m_state = State::Candidate;
    ++m_currentTerm;
    m_votedFor = m_id;
    int votesGranted = 1; // vote for self

    // Send RequestVote to all peers
    Message rv;
    rv.type = MessageType::RequestVote;
    rv.term = m_currentTerm;
    rv.src = m_id;
    rv.lastLogIndex = lastLogIndex();
    rv.lastLogTerm = lastLogTerm();

    for (NodeId peer : m_peers) {
        if (peer == m_id) continue;
        rv.dst = peer;
        sendMessage(rv);
    }

    // Store votes count in a lambda capture (simple approach)
    // In this reference implementation, we process responses synchronously in handleRequestVoteResponse.
    (void)votesGranted; // silence unused warning
}

void RaftNode::becomeLeader() {
    m_state = State::Leader;
    // Initialize nextIndex and matchIndex
    Index next = lastLogIndex() + 1;
    for (NodeId peer : m_peers) {
        if (peer == m_id) continue;
        m_nextIndex[peer] = next;
        m_matchIndex[peer] = 0;
    }
    // Immediately send empty AppendEntries as heartbeat
    Message hb;
    hb.type = MessageType::AppendEntries;
    hb.term = m_currentTerm;
    hb.src = m_id;
    hb.prevLogIndex = lastLogIndex();
    hb.prevLogTerm = lastLogTerm();
    hb.leaderCommit = m_commitIndex;
    broadcast(hb);
    resetHeartbeatTimer();
}

void RaftNode::resetElectionTimer() {
    std::uniform_int_distribution<> dist(kElectionTimeoutMin, kElectionTimeoutMax);
    m_electionTimeout = dist(gen);
}

void RaftNode::resetHeartbeatTimer() {
    m_heartbeatTimeout = kHeartbeatInterval;
}

void RaftNode::sendMessage(const Message& msg) {
    m_outbox.push(msg);
}

void RaftNode::broadcast(const Message& msg) {
    for (NodeId peer : m_peers) {
        if (peer == m_id) continue;
        Message copy = msg;
        copy.dst = peer;
        sendMessage(copy);
    }
}

void RaftNode::handleRequestVote(const Message& msg) {
    Message resp;
    resp.type = MessageType::RequestVoteResponse;
    resp.term = m_currentTerm;
    resp.src = m_id;
    resp.dst = msg.src;

    bool grant = false;
    if (msg.term < m_currentTerm) {
        grant = false;
    } else {
        bool notVoted = (!m_votedFor.has_value() || m_votedFor.value() == msg.src);
        bool upToDate = (msg.lastLogTerm > lastLogTerm()) ||
                        (msg.lastLogTerm == lastLogTerm() && msg.lastLogIndex >= lastLogIndex());
        if (notVoted && upToDate) {
            grant = true;
            m_votedFor = msg.src;
            resetElectionTimer();
        }
    }
    resp.voteGranted = grant;
    sendMessage(resp);
}

void RaftNode::handleRequestVoteResponse(const Message& msg) {
    if (m_state != State::Candidate) return;
    if (msg.term > m_currentTerm) {
        becomeFollower(msg.term);
        return;
    }
    if (msg.term < m_currentTerm) return;

    static std::unordered_map<NodeId, int> voteCount; // per term
    if (msg.voteGranted) {
        voteCount[m_id]++; // use node id as key for term (simplified)
        int needed = (m_peers.size() / 2) + 1;
        if (voteCount[m_id] >= needed) {
            becomeLeader();
            voteCount.clear();
        }
    }
}

void RaftNode::handleAppendEntries(const Message& msg) {
    Message resp;
    resp.type = MessageType::AppendEntriesResponse;
    resp.term = m_currentTerm;
    resp.src = m_id;
    resp.dst = msg.src;
    resp.success = false;

    if (msg.term < m_currentTerm) {
        sendMessage(resp);
        return;
    }

    // Become follower if leader's term is newer
    if (msg.term > m_currentTerm) {
        becomeFollower(msg.term);
    }

    // Reset election timer on valid AppendEntries
    resetElectionTimer();

    // Consistency check
    if (msg.prevLogIndex >= m_log.size() ||
        m_log[msg.prevLogIndex].term != msg.prevLogTerm) {
        sendMessage(resp);
        return;
    }

    // Append any new entries
    Index idx = msg.prevLogIndex + 1;
    for (const auto& entry : msg.entries) {
        if (idx < m_log.size()) {
            if (m_log[idx].term != entry.term) {
                // Delete conflict entry and all that follow
                m_log.resize(idx);
                m_log.push_back(entry);
            }
        } else {
            m_log.push_back(entry);
        }
        ++idx;
    }

    // Update commit index
    if (msg.leaderCommit > m_commitIndex) {
        m_commitIndex = std::min(msg.leaderCommit, static_cast<Index>(m_log.size() - 1));
        applyLogEntries();
    }

    resp.success = true;
    sendMessage(resp);
}

void RaftNode::handleAppendEntriesResponse(const Message& msg) {
    if (m_state != State::Leader) return;
    if (msg.term > m_currentTerm) {
        becomeFollower(msg.term);
        return;
    }
    if (!msg.success) {
        // Decrement nextIndex and retry
        auto& nextIdx = m_nextIndex[msg.src];
        if (nextIdx > 1
);

}
}
}
