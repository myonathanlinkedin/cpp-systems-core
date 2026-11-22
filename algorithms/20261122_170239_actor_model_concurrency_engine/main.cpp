#include <iostream>
#include <cassert>
#include <chrono>
#include <atomic>

#include "types.hpp"
#include "core.hpp"

int main() {
    std::atomic<int> counter{0};

    Actor actorA;
    Actor actorB;

    // Actor A increments the counter
    actorA.send([&](){ counter++; });

    // Actor B sends 100 messages to Actor A
    for (int i = 0; i < 100; ++i) {
        actorB.send([&](){ actorA.send([&](){ counter++; }); });
    }

    // Wait until all messages are processed
    while (counter.load() < 101) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    assert(counter.load() == 101);
    std::cout << "All messages processed: counter = " << counter.load() << std::endl;

    actorA.stop();
    actorB.stop();

    return 0;
}
