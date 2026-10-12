#pragma once
#include <vector>

namespace timing {

struct Task {
    int id;          // Unique identifier
    int period;      // Time between releases
    int execution;   // Execution time per job
    int deadline;    // Relative deadline from release
    int next_release; // Next release time
    int remaining;   // Remaining execution time for current job

    Task(int id_, int period_, int execution_, int deadline_)
        : id(id_), period(period_), execution(execution_), deadline(deadline_),
          next_release(0), remaining(0) {};

    Task() = default;
};

} // namespace timing
