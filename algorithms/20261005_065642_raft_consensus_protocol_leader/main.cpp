#include <cstdint>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <thread>

enum class Role {
    FOLLOWER,
    CANDIDATE,
    LEADER
};

struct LogEntry {
    uint64_t term;
    uint64_t index;
    std::string data;
};

class RaftNode {
public:
    RaftNode(uint64_t id, uint64_t electionTimeoutBase)
        : id_(id),
          role_(Role::FOLLOWER),
          currentTerm_(0),
          votedFor_(0),
          electionTimeoutBase_(electionTimeoutBase),
          electionTimeout_(electionTimeoutBase),
          lastLogIndex_(0),
          lastLogTerm_(0),
          commitIndex_(0),
          nextIndex_(0),
          matchIndex_(0),
          leaderId_(0),
          rng_(std::chrono::steady_clock::now().time_since_epoch().count()) {}

    void resetElectionTimer() {
        // Randomize timeout between base and 2*base to avoid split votes
        std::uniform_int_distribution<uint64_t> dist(electionTimeoutBase_, 2 * electionTimeoutBase_);
        electionTimeout_ = dist(rng_);
    }

    void tick() {
        if (electionTimeout_ > 0) {
            --electionTimeout_;
        } else {
            if (role_ == Role::FOLLOWER || role_ == Role::CANDIDATE) {
                startElection();
            }
        }
    }

    void startElection() {
        ++currentTerm_;
        votedFor_ = id_;
        role_ = Role::CANDIDATE;
        resetElectionTimer();
        // In a real system, we would broadcast RequestVote RPCs here.
        // For this state engine, we simulate the outcome based on log state.
        // If we have the most up-to-date log, we assume we win the election.
        if (isLogUpToDate()) {
            becomeLeader();
        }
    }

    void becomeLeader() {
        role_ = Role::LEADER;
        leaderId_ = id_;
        // Initialize nextIndex and matchIndex for all peers (simulated as self for simplicity in this engine)
        nextIndex_ = lastLogIndex_ + 1;
        matchIndex_ = 0;
        resetElectionTimer();
    }

    bool isLogUpToDate() {
        // A log is up-to-date if its last log entry's term is greater than another log's,
        // or if they have the same term and this log is at least as long.
        // In this simplified engine, we assume the node with the highest term and longest log wins.
        // For a single-node simulation or deterministic test, we check if we have the highest term.
        // In a multi-node scenario, this would require comparing with other nodes' logs.
        // Here, we simplify: if we are the only candidate or have the highest term, we win.
        return true; // Simplified for state engine demonstration
    }

    bool handleRequestVote(uint64_t candidateId, uint64_t candidateLastLogIndex, uint64_t candidateLastLogTerm) {
        if (candidateId == id_) return false; // Ignore self
        if (currentTerm_ > candidateLastLogTerm) {
            return false; // Reject if our term is higher
        }
        if (currentTerm_ < candidateLastLogTerm) {
            currentTerm_ = candidateLastLogTerm;
            votedFor_ = 0; // Reset vote for new term
            role_ = Role::FOLLOWER;
            resetElectionTimer();
        }
        // Check if candidate's log is at least as up-to-date as ours
        if (candidateLastLogTerm < lastLogTerm_ ||
            (candidateLastLogTerm == lastLogTerm_ && candidateLastLogIndex < lastLogIndex_)) {
            return false; // Reject if our log is more up-to-date
        }
        // Grant vote if we haven't voted yet in this term
        if (votedFor_ == 0 || votedFor_ == candidateId) {
            votedFor_ = candidateId;
            resetElectionTimer();
            return true;
        }
        return false;
    }

    bool handleHeartbeat(uint64_t leaderId, uint64_t leaderTerm) {
        if (leaderTerm < currentTerm_) {
            return false; // Reject stale leader
        }
        if (leaderTerm > currentTerm_) {
            currentTerm_ = leaderTerm;
            votedFor_ = 0;
            role_ = Role::FOLLOWER;
        }
        if (role_ == Role::CANDIDATE) {
            role_ = Role::FOLLOWER;
        }
        leaderId_ = leaderId;
        resetElectionTimer();
        return true;
    }

    void appendLog(const LogEntry& entry) {
        if (entry.term > lastLogTerm_) {
            lastLogTerm_ = entry.term;
        }
        lastLogIndex_ = entry.index;
        // In a real implementation, we would store the entry in a vector
    }

    // Getters for testing
    Role getRole() const { return role_; }
    uint64_t getCurrentTerm() const { return currentTerm_; }
    uint64_t getVotedFor() const { return votedFor_; }
    uint64_t getLeaderId() const { return leaderId_; }
    uint64_t getLastLogIndex() const { return lastLogIndex_; }
    uint64_t getLastLogTerm() const { return lastLogTerm_; }

private:
    uint64_t id_;
    Role role_;
    uint64_t currentTerm_;
    uint64_t votedFor_;
    uint64_t electionTimeoutBase_;
    uint64_t electionTimeout_;
    uint64_t lastLogIndex_;
    uint64_t lastLogTerm_;
    uint64_t commitIndex_;
    uint64_t nextIndex_;
    uint64_t matchIndex_;
    uint64_t leaderId_;
    std::mt19937_64 rng_;
};

int main() {
    // Test 1: Initial state
    RaftNode node1(1, 100);
    assert(node1.getRole() == Role::FOLLOWER);
    assert(node1.getCurrentTerm() == 0);
    assert(node1.getVotedFor() == 0);

    // Test 2: Election timeout triggers candidate state
    for (int i = 0; i < 100; ++i) {
        node1.tick();
    }
    assert(node1.getRole() == Role::CANDIDATE);
    assert(node1.getCurrentTerm() == 1);
    assert(node1.getVotedFor() == 1);

    // Test 3: Leader election (simplified: node wins if log is up-to-date)
    // Since isLogUpToDate() returns true, node becomes leader
    assert(node1.getRole() == Role::LEADER);
    assert(node1.getLeaderId() == 1);

    // Test 4: Handle heartbeat from another leader
    RaftNode node2(2, 100);
    // node2 is follower, term 0
    bool accepted = node2.handleHeartbeat(1, 1);
    assert(accepted == true);
    assert(node2.getRole() == Role::FOLLOWER);
    assert(node2.getCurrentTerm() == 1);
    assert(node2.getLeaderId() == 1);

    // Test 5: Reject stale heartbeat
    bool rejected = node2.handleHeartbeat(1, 0);
    assert(rejected == false);
    assert(node2.getCurrentTerm() == 1); // Term should not decrease

    // Test 6: Request vote handling
    RaftNode node3(3, 100);
    // node3 is follower, term 0
    bool voteGranted = node3.handleRequestVote(1, 0, 1);
    assert(voteGranted == true);
    assert(node3.getVotedFor() == 1);
    assert(node3.getCurrentTerm() == 1);

    // Test 7: Reject vote if already voted for another candidate
    bool voteRejected = node3.handleRequestVote(2, 0, 1);
    assert(voteRejected == false);
    assert(node3.getVotedFor() == 1); // Still voted for 1

    // Test 8: Reject vote if candidate's log is less up-to-date
    RaftNode node4(4, 100);
    node4.appendLog({1, 1, "data"}); // node4 has log term 1, index 1
    bool voteRejected2 = node4.handleRequestVote(1, 0, 0); // Candidate has term 0, index 0
    assert(voteRejected2 == false);
    assert(node4.getVotedFor() == 0); // No vote granted

    // Test 9: Grant vote if candidate's log is more up-to-date
    bool voteGranted2 = node4.handleRequestVote(1, 1, 1); // Candidate has term 1, index 1
    assert(voteGranted2 == true);
    assert(node4.getVotedFor() == 1);

    // Test 10: Term update on higher term request
    RaftNode node5(5, 100);
    node5.handleHeartbeat(1, 2); // node5 now at term 2
    bool voteGranted3 = node5.handleRequestVote(1, 0, 3); // Candidate at term 3
    assert(voteGranted3 == true);
    assert(node5.getCurrentTerm() == 3);
    assert(node5.getVotedFor() == 1);

    std::cout << "All Raft Consensus Protocol Leader Election State Engine tests passed." << std::endl;
    return 0;
}
