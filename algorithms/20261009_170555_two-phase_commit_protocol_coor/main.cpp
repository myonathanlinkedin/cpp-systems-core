#include <cassert>

#include <iostream>
#include "types.hpp"
#include "core.hpp"

using namespace tpc;

int main() {
    // ---------- Test 1: All participants commit ----------
    {
        Coordinator coord;
        auto p1 = std::make_shared<Participant>(1, true);
        auto p2 = std::make_shared<Participant>(2, true);
        auto p3 = std::make_shared<Participant>(3, true);
        coord.addParticipant(p1);
        coord.addParticipant(p2);
        coord.addParticipant(p3);
        coord.startTransaction();

        assert(coord.state() == CoordinatorState::Committed);
        assert(p1->state() == ParticipantState::Committed);
        assert(p2->state() == ParticipantState::Committed);
        assert(p3->state() == ParticipantState::Committed);
        std::cout << "Test 1 passed: all commit.\n";
    }

    // ---------- Test 2: One participant aborts ----------
    {
        Coordinator coord;
        auto p1 = std::make_shared<Participant>(1, true);
        auto p2 = std::make_shared<Participant>(2, false); // will abort
        auto p3 = std::make_shared<Participant>(3, true);
        coord.addParticipant(p1);
        coord.addParticipant(p2);
        coord.addParticipant(p3);
        coord.startTransaction();

        assert(coord.state() == CoordinatorState::Aborted);
        assert(p1->state() == ParticipantState::Aborted);
        assert(p2->state() == ParticipantState::Aborted);
        assert(p3->state() == ParticipantState::Aborted);
        std::cout << "Test 2 passed: abort on single negative vote.\n";
    }

    // ---------- Test 3: No participants (trivial commit) ----------
    {
        Coordinator coord;
        coord.startTransaction();
        assert(coord.state() == CoordinatorState::Committed);
        std::cout << "Test 3 passed: commit with zero participants.\n";
    }

    // ---------- Test 4: Verify state transitions ----------
    {
        Participant p(42, true);
        assert(p.state() == ParticipantState::Init);
        Coordinator dummy;
        dummy.addParticipant(std::make_shared<Participant>(p)); // not used, just to keep API consistent
        // Simulate prepare manually
        p.onPrepare(dummy);
        assert(p.state() == ParticipantState::VotedCommit);
        p.onDecision(true);
        assert(p.state() == ParticipantState::Committed);
        std::cout << "Test 4 passed: participant state machine.\n";
    }

    std::cout << "All tests passed successfully.\n";
    return 0;
}
