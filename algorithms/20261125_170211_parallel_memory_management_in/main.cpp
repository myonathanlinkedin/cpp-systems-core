#include "types.hpp"
#include "engine.cpp"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

using namespace mem;

void single_thread_test() {
    MemoryPool<TestData> pool;
    const int N = 1000;
    std::vector<TestData*> ptrs;
    ptrs.reserve(N);

    for (int i = 0; i < N; ++i) {
        TestData* p = pool.allocate();
        p->id = i;
        p->value = i * 0.5;
        ptrs.push_back(p);
    }

    for (int i = 0; i < N; ++i) {
        assert(ptrs[i]->id == i);
        assert(ptrs[i]->value == i * 0.5);
    }

    for (auto p : ptrs) {
        pool.deallocate(p);
    }

    // Re‑allocate to verify that objects are re‑constructed to default state
    for (int i = 0; i < N; ++i) {
        TestData* p = pool.allocate();
        assert(p->id == 0 && p->value == 0.0);
        pool.deallocate(p);
    }
}

void parallel_test() {
    MemoryPool<TestData> pool;
    const unsigned int thread_cnt = std::thread::hardware_concurrency()
                                    ? std::thread::hardware_concurrency()
                                    : 4;
    const int ops_per_thread = 10000;

    auto worker = [&](int tid) {
        std::vector<TestData*> local;
        local.reserve(ops_per_thread);
        for (int i = 0; i < ops_per_thread; ++i) {
            TestData* p = pool.allocate();
            p->id = tid;
            p->value = static_cast<double>(i);
            local.push_back(p);
        }
        // Deallocate in reverse order to increase contention on the free list
        for (int i = ops_per_thread - 1; i >= 0; --i) {
            pool.deallocate(local[i]);
        }
    };

    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < thread_cnt; ++t) {
        threads.emplace_back(worker, static_cast<int>(t));
    }
    for (auto& th : threads) {
        th.join();
    }

    // Verify pool remains functional after heavy parallel use
    for (int i = 0; i < 1000; ++i) {
        TestData* p = pool.allocate();
        assert(p != nullptr);
        pool.deallocate(p);
    }
}

int main() {
    single_thread_test();
    parallel_test();
    std::cout << "All tests passed.\n";
    return 0;
}
