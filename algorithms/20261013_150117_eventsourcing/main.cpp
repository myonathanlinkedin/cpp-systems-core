#include <iostream>
#include <cassert>
#include "core.cpp"

using namespace es;

void testEmptyReplay() {
    EventStore store;
    State s = store.replay();
    assert(s.counter == 0);
}

void testAppendAndReplay() {
    EventStore store;
    store.append({EventType::Increment, 5});
    store.append({EventType::Decrement, 2});
    store.append({EventType::Set, 10});
    State s = store.replay();
    assert(s.counter == 10);
}

void testSnapshotReplay() {
    EventStore store;
    store.enableSnapshot(2); // snapshot every 2 events
    store.append({EventType::Increment, 1}); // idx 0
    store.append({EventType::Increment, 2}); // idx 1 -> snapshot
    store.append({EventType::Decrement, 1}); // idx 2
    store.append({EventType::Increment, 4}); // idx 3 -> snapshot
    State s_no_snap = store.replay();
    State s_snap = store.replayWithSnapshot();
    assert(s_no_snap.counter == s_snap.counter);
    assert(s_no_snap.counter == 6);
}

void testSnapshotEnableAfterEvents() {
    EventStore store;
    store.append({EventType::Increment, 3});
    store.append({EventType::Increment, 4});
    store.enableSnapshot(1); // now every event gets a snapshot
    store.append({EventType::Decrement, 2});
    State s = store.replayWithSnapshot();
    assert(s.counter == 5);
}

void testUnknownEventTypeSafety() {
    EventStore store;
    // Cast an invalid integer to EventType to simulate corruption.
    Event bad{static_cast<EventType>(999), 42};
    store.append(bad);
    State s = store.replay();
    // No change expected because apply ignores unknown types.
    assert(s.counter == 0);
}

int main() {
    testEmptyReplay();
    testAppendAndReplay();
    testSnapshotReplay();
    testSnapshotEnableAfterEvents();
    testUnknownEventTypeSafety();

    std::cout << "All Eventsourcing tests passed.\n";
    return 0;
}
