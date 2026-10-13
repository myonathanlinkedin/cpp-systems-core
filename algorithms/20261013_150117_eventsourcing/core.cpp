#include <vector>
#include <utility>
#include <cstddef>

namespace es {

enum class EventType {
    Increment,
    Decrement,
    Set
};

struct Event {
    EventType type = EventType::Increment;
    int value = 0;
};

struct State {
    int counter = 0;
};

class EventStore {
public:
    EventStore() = default;

    // Append a new event to the log.
    void append(const Event& e) {
        _events.push_back(e);
        apply(e, _currentState);
        maybeCreateSnapshot(_events.size() - 1, _currentState);
    };

    // Retrieve the immutable event log.
    const std::vector<Event>& events() const {
        return _events;
    }

    // Replay all events from scratch (no snapshots).
    State replay() const {
        State s;
        for (const auto& e : _events) {
            apply(e, s);
        }
        return s;
    }

    // Enable snapshotting; a snapshot is taken every 'interval' events.
    void enableSnapshot(std::size_t interval) {
        _snapshotInterval = interval;
        _snapshots.clear();
        if (interval == 0) return;
        State s;
        for (std::size_t i = 0; i < _events.size(); ++i) {
            apply(_events[i], s);
            if ((i + 1) % interval == 0) {
                _snapshots.emplace_back(i, s);
            }
        }
        _currentState = s;
    }

    // Replay using the latest snapshot before the target index.
    State replayWithSnapshot() const {
        if (_snapshots.empty()) {
            return replay();
        }
        // Use the last snapshot.
        const auto& [snapIdx, snapState] = _snapshots.back();
        State s = snapState;
        for (std::size_t i = snapIdx + 1; i < _events.size(); ++i) {
            apply(_events[i], s);
        }
        return s;
    }

private:
    std::vector<Event> _events;
    std::size_t _snapshotInterval = 0;
    std::vector<std::pair<std::size_t, State>> _snapshots;
    State _currentState; // state after processing all events (kept for convenience)

    // Apply a single event to a given state.
    static void apply(const Event& e, State& s) {
        switch (e.type) {
            case EventType::Increment:
                s.counter += e.value;
                break;
            case EventType::Decrement:
                s.counter -= e.value;
                break;
            case EventType::Set:
                s.counter = e.value;
                break;
            default:
                // Unknown event type – no effect.
                break;
        }
    }

    // Create a snapshot if the interval condition is met.
    void maybeCreateSnapshot(std::size_t index, const State& s) {
        if (_snapshotInterval == 0) return;
        if ((index + 1) % _snapshotInterval == 0) {
            _snapshots.emplace_back(index, s);
        }
    }
};

} // namespace es
