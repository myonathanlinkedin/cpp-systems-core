#pragma once
#include "types.hpp"
#include <vector>
#include <queue>
#include <utility>

namespace timing {

class Scheduler {
public:
    Scheduler() = default;

    // Add a task to the system. The task is copied.
    void addTask(const Task& t);

    // Run a deterministic simulation for total_time time units.
    // Returns true if all deadlines are met, false otherwise.
    bool run(int total_time);

    // Retrieve the schedule: each pair is (time, task_id). task_id == -1 denotes idle.
    const std::vector<std::pair<int, int>>& getSchedule() const;

private:
    std::vector<Task> tasks_;                     // All tasks known to the scheduler
    std::vector<std::pair<int, int>> schedule_;   // (time, task_id) log

    struct ReadyTask {
        int deadline_time; // absolute deadline
        int task_index;    // index into tasks_ vector

        // priority_queue is a max‑heap; invert comparison for earliest deadline first
        bool operator<(const ReadyTask& other) const {
            return deadline_time > other.deadline_time;
        };
    };
};

} // namespace timing
