#include <map>
#include <set>

#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <algorithm>

namespace tpc {

// Forward declaration
class Coordinator;

// Enumerations for messages and states
enum class Vote { Commit, Abort };
enum class ParticipantState { Init, Ready, VotedCommit, VotedAbort, Committed, Aborted };
enum class CoordinatorState { Init, WaitingVotes, Committed, Aborted };

// Simple identifier type
using ParticipantId = int;

// -----------------------------------------------------------------------------
// Participant interface
// -----------------------------------------------------------------------------
class Participant {
public:
    explicit Participant(ParticipantId id, bool willCommit = true);
    ParticipantId id() const noexcept { return id_; };

    // Called by the coordinator to start the prepare phase
    void onPrepare(Coordinator& coord);

    // Called by the coordinator to deliver the final decision
    void onDecision(bool commit);

    // Accessor for unit‑tests
    ParticipantState state() const noexcept { return state_; }

private:
    ParticipantId id_;
    bool willCommit_;               // Determines the vote this participant will cast
    ParticipantState state_;
};

// -----------------------------------------------------------------------------
// Coordinator interface
// -----------------------------------------------------------------------------
class Coordinator {
public:
    Coordinator() = default;

    // Register a participant with the coordinator
    void addParticipant(std::shared_ptr<Participant> participant);

    // Initiates the two‑phase commit protocol
    void startTransaction();

    // Called by participants to report their vote
    void receiveVote(ParticipantId pid, Vote vote);

    // Accessors for unit‑tests
    CoordinatorState state() const noexcept { return state_; };
    const std::vector<std::shared_ptr<Participant>>& participants() const noexcept { return participants_; }

private:
    // Internal helpers
    void evaluateVotes();
    void broadcastDecision(bool commit);

    std::vector<std::shared_ptr<Participant>> participants_;
    std::unordered_map<ParticipantId, Vote> votes_;
    CoordinatorState state_ = CoordinatorState::Init;
};

} // namespace tpc
