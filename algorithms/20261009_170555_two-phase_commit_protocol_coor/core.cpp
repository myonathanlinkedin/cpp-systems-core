#include <memory>
#include <cassert>

#include "core.hpp"

namespace tpc {

// --------------------------- Participant ------------------------------------
Participant::Participant(ParticipantId id, bool willCommit)
    : id_(id), willCommit_(willCommit), state_(ParticipantState::Init) {}

void Participant::onPrepare(Coordinator& coord) {
    assert(state_ == ParticipantState::Init || state_ == ParticipantState::Ready);
    state_ = ParticipantState::Ready;
    // Decide vote based on internal flag
    Vote vote = willCommit_ ? Vote::Commit : Vote::Abort;
    // Record own vote state
    state_ = (vote == Vote::Commit) ? ParticipantState::VotedCommit : ParticipantState::VotedAbort;
    // Report vote back to coordinator
    coord.receiveVote(id_, vote);
}

void Participant::onDecision(bool commit) {
    // Decision must be received after a vote
    assert(state_ == ParticipantState::VotedCommit || state_ == ParticipantState::VotedAbort);
    if (commit) {
        state_ = ParticipantState::Committed;
    } else {
        state_ = ParticipantState::Aborted;
    }
}

// --------------------------- Coordinator ------------------------------------
void Coordinator::addParticipant(std::shared_ptr<Participant> participant) {
    assert(state_ == CoordinatorState::Init); // No dynamic changes after start
    participants_.push_back(std::move(participant));
}

void Coordinator::startTransaction() {
    assert(state_ == CoordinatorState::Init);
    // Edge case: no participants -> commit trivially
    if (participants_.empty()) {
        state_ = CoordinatorState::Committed;
        return;
    }
    state_ = CoordinatorState::WaitingVotes;
    // Phase 1: send prepare to all participants
    for (const auto& p : participants_) {
        p->onPrepare(*this);
    }
    // After all votes have been collected (synchronous simulation), evaluate
    evaluateVotes();
}

void Coordinator::receiveVote(ParticipantId pid, Vote vote) {
    // Record vote; duplicate votes from same participant are illegal
    assert(votes_.find(pid) == votes_.end());
    votes_[pid] = vote;
}

void Coordinator::evaluateVotes() {
    assert(state_ == CoordinatorState::WaitingVotes);
    // If any participant voted abort, the transaction aborts
    bool allCommit = std::all_of(votes_.begin(), votes_.end(),
                                [](const auto& kv) { return kv.second == Vote::Commit; });
    if (allCommit) {
        state_ = CoordinatorState::Committed;
    } else {
        state_ = CoordinatorState::Aborted;
    }
    broadcastDecision(allCommit);
}

void Coordinator::broadcastDecision(bool commit) {
    for (const auto& p : participants_) {
        p->onDecision(commit);
    }
}

} // namespace tpc
