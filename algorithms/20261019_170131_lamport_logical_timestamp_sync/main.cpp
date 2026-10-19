#include <algorithm>

#include "types.hpp"
#include "core.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace lamport;

int main() {
    // Test 1: Simple local events
    {
        LamportClock clk;
        assert(clk.now() == 0);
        assert(clk.tick() == 1);
        assert(clk.tick() == 2);
        assert(clk.now() == 2);
    }

    // Test 2: Receive remote timestamps
    {
        LamportClock clk;
        clk.tick();               // local event -> 1
        clk.receive(5);           // remote > local -> max(1,5)+1 = 6
        assert(clk.now() == 6);
        clk.receive(3);           // remote < local -> max(6,3)+1 = 7
        assert(clk.now() == 7);
    }

    // Test 3: Engine send/receive ordering
    {
        LamportEngine procA, procB;

        // A sends two messages
        Message m1 = procA.send("A1");
        Message m2 = procA.send("A2");

        // B receives first message
        procB.receive(m1);
        assert(procB.clock().now() == std::max(0, m1.timestamp) + 1);

        // B sends a reply
        Message r1 = procB.send("B1");

        // A receives B's reply
        procA.receive(r1);
        // A's clock must be > both its previous timestamp and r1.timestamp
        assert(procA.clock().now() > m2.timestamp);
        assert(procA.clock().now() > r1.timestamp);
    }

    // Test 4: Causal chain across three processes
    {
        LamportEngine p1, p2, p3;

        // p1 -> p2
        Message a = p1.send("msg1");
        p2.receive(a);

        // p2 -> p3
        Message b = p2.send("msg2");
        p3.receive(b);

        // p3 -> p1
        Message c = p3.send("msg3");
        p1.receive(c);

        // Verify monotonicity
        assert(p1.clock().now() > a.timestamp);
        assert(p2.clock().now() > a.timestamp);
        assert(p3.clock().now() > b.timestamp);
        assert(p1.clock().now() > c.timestamp);
    }

    // Test 5: No timestamp regression
    {
        LamportEngine engine;
        for (int i = 0; i < 100; ++i) {
            Message m = engine.send("tick");
            assert(m.timestamp == i + 1);
        }
    }

    std::cout << "All Lamport logical clock tests passed.\n";
    return 0;
}
