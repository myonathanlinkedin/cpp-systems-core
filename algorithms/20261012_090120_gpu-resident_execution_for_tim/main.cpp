#include "types.hpp"
#include "core.hpp"
#include <iostream>
#include <cassert>

using namespace timing;

void test_single_task() {
    Scheduler sch;
    sch.addTask(Task(1, 5, 2, 5)); // id=1, period=5, exec=2, deadline=5
    bool ok = sch.run(20);
    assert(ok);
    const auto& sched = sch.getSchedule();
    // Expect 4 executions of task 1 (20 / 5 periods)
    int exec_count = 0;
    for (const auto& p : sched) {
        if (p.second == 1) ++exec_count;
    }
    assert(exec_count == 4);
}

void test_two_tasks_meet() {
    Scheduler sch;
    sch.addTask(Task(1, 4, 1, 4));
    sch.addTask(Task(2, 5, 2, 5));
    bool ok = sch.run(20);
    assert(ok);
    const auto& sched = sch.getSchedule();
    // Simple sanity: total executed time should equal sum of all jobs' execution times
    int expected = (20 / 4) * 1 + (20 / 5) * 2;
    int actual = 0;
    for (const auto& p : sched) {
        if (p.second != -1) ++actual;
    }
    assert(actual == expected);
}

void test_overload_miss() {
    Scheduler sch;
    sch.addTask(Task(1, 3, 2, 3));
    sch.addTask(Task(2, 4, 2, 4));
    bool ok = sch.run(12);
    assert(!ok); // Should miss a deadline
}

void test_zero_tasks() {
    Scheduler sch;
    bool ok = sch.run(10);
    assert(ok);
    const auto& sched = sch.getSchedule();
    for (const auto& p : sched) {
        assert(p.second == -1); // all idle
    }
}

void test_zero_execution() {
    Scheduler sch;
    sch.addTask(Task(1, 5, 0, 5));
    bool ok = sch.run(15);
    assert(ok);
    const auto& sched = sch.getSchedule();
    // No execution time should be recorded for task 1
    for (const auto& p : sched) {
        assert(p.second == -1);
    }
}

int main() {
    test_single_task();
    test_two_tasks_meet();
    test_overload_miss();
    test_zero_tasks();
    test_zero_execution();

    std::cout << "All timing‑predictable scheduler tests passed.\n";
    return 0;
}
