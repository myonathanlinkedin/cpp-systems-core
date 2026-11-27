#include <vector>
#include <chrono>
#include <atomic>

#include <iostream>
#include <cassert>
#include "types.hpp"
#include "core.hpp"

using namespace actor;

int main() {
    // Basic sanity test: single actor processes messages in order
    {
        ActorEngine engine(2);
        ActorId aid = engine.create_actor();
        Counter cnt;

        engine.send(aid, [&cnt] { cnt.increment(); });
        engine.send(aid, [&cnt] { cnt.add(5); });
        engine.send(aid, [&cnt] { cnt.increment(); });

        // Allow some time for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        engine.shutdown();

        assert(cnt.value == 7 && "Counter should be 7 after ordered processing");
    }

    // Concurrency test: multiple actors increment shared atomic counter
    {
        ActorEngine engine(4);
        const std::size_t actor_num = 8;
        const std::size_t msgs_per_actor = 1000;
        std::atomic<int> atomic_cnt{0};

        std::vector<ActorId> ids;
        for (std::size_t i = 0; i < actor_num; ++i) {
            ids.push_back(engine.create_actor());
        }

        for (auto id : ids) {
            for (std::size_t j = 0; j < msgs_per_actor; ++j) {
                engine.send(id, [&atomic_cnt] { atomic_cnt.fetch_add(1, std::memory_order_relaxed); });
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        engine.shutdown();

        int expected = static_cast<int>(actor_num * msgs_per_actor);
        assert(atomic_cnt.load() == expected && "All messages should be processed exactly once");
    }

    // Stress test: rapid fire messages and ensure no deadlock
    {
        ActorEngine engine(8);
        ActorId aid = engine.create_actor();
        Counter cnt;

        const std::size_t total = 5000;
        for (std::size_t i = 0; i < total; ++i) {
            engine.send(aid, [&cnt] { cnt.increment(); });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        engine.shutdown();

        assert(cnt.value == static_cast<int>(total) && "All rapid messages must be processed");
    }

    std::cout << "All actor model tests passed.\n";
    return 0;
}
