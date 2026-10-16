#include <vector>
#include <queue>
#include <utility>

#include "core.hpp"
#include <algorithm>
#include <cassert>

namespace timing {

void Scheduler::addTask(const Task& t) {
    // Ensure task parameters are non‑negative
    assert(t.period > 0);
    assert(t.execution >= 0);
    assert(t.deadline > 0);
    tasks_.push_back(t);
}

bool Scheduler::run(int total_time) {
    assert(total_time >= 0);
    schedule_.clear();

    // Priority queue of ready jobs ordered by earliest absolute deadline
    std::priority_queue<ReadyTask> ready;

    // Index of the task currently executing in this time slice (-1 if idle)
    int current_task_idx = -1;

    for (int time = 0; time < total_time; ++time) {
        // 1. Release new jobs whose release time equals current time
        for (size_t i = 0; i < tasks_.size(); ++i) {
            Task& task = tasks_[i];
            if (time == task.next_release) {
                // Job released
                task.remaining = task.execution;
                task.next_release += task.period;
                // If execution time is zero, job completes instantly; still need deadline check later
                if (task.remaining > 0) {
                    ready.push({time + task.deadline, static_cast<int>(i)});
                }
            }
        }

        // 2. If a job was running in the previous slice and still has work, re‑insert it
        if (current_task_idx != -1 && tasks_[current_task_idx].remaining > 0) {
            ready.push({time + tasks_[current_task_idx].deadline - (tasks_[current_task_idx].next_release - tasks_[current_task_idx].period - tasks_[current_task_idx].period),
                        current_task_idx});
        }

        // 3. Deadline miss detection: any ready job whose deadline <= current time is a miss
        if (!ready.empty()) {
            const ReadyTask& top = ready.top();
            if (top.deadline_time <= time) {
                // Missed deadline
                return false;
            }
        }

        // 4. Select the job with the earliest deadline (if any)
        if (!ready.empty()) {
            ReadyTask rt = ready.top();
            ready.pop();
            Task& task = tasks_[rt.task_index];
            // Execute one time unit
            --task.remaining;
            current_task_idx = rt.task_index;
            schedule_.emplace_back(time, task.id);
            // If job still has remaining execution, it will be re‑inserted at the start of next slice
        } else {
            // CPU idle this time unit
            current_task_idx = -1;
            schedule_.emplace_back(time, -1);
        }
    }

    // After simulation, ensure no pending jobs missed their deadlines
    while (!ready.empty()) {
        const ReadyTask& rt = ready.top();
        if (rt.deadline_time <= total_time) {
            return false;
        }
        ready.pop();
    }
    if (current_task_idx != -1 && tasks_[current_task_idx].remaining > 0) {
        // Remaining work after simulation horizon implies a miss
        return false;
    }

    return true;
}

const std::vector<std::pair<int, int>>& Scheduler::getSchedule() const {
    return schedule_;
}

} // namespace timing
