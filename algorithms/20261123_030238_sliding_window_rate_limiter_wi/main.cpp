#include <iostream>
#include <cassert>
#include "core.cpp"

int main() {
    // Test 1: Basic allowance within capacity.
    {
        SlidingWindowRateLimiter limiter(5, 5.0, 1.0); // 5 tokens, 5/s refill, 1‑s window
        double t = 0.0;
        for (int i = 0; i < 5; ++i) {
            assert(limiter.allow(t) && "First 5 requests should be allowed");
        }
        // Sixth request should be rejected because bucket empty and window full.
        assert(!limiter.allow(t) && "Sixth request at same time must be rejected");
    }

    // Test 2: Token refill after time passes.
    {
        SlidingWindowRateLimiter limiter(3, 1.0, 2.0); // 3 tokens max, 1 token/s, 2‑s window
        double t = 0.0;
        // Consume all tokens.
        for (int i = 0; i < 3; ++i) {
            assert(limiter.allow(t));
        }
        // No tokens left.
        assert(!limiter.allow(t));
        // Advance 1.5 seconds: 1.5 tokens should be added (but bucket caps at 3).
        t += 1.5;
        // Only one request can be allowed because we have 1.5 tokens (>=1).
        assert(limiter.allow(t));
        // Still have 0.5 token left → next request denied.
        assert(!limiter.allow(t));
        // Advance another 1 second (total 2.5s elapsed): another 1 token added.
        t += 1.0;
        assert(limiter.allow(t));
    }

    // Test 3: Sliding window burst control.
    {
        SlidingWindowRateLimiter limiter(4, 10.0, 0.5); // high refill, short window
        double t = 0.0;
        // Issue 4 requests instantly – allowed (window size = capacity).
        for (int i = 0; i < 4; ++i) {
            assert(limiter.allow(t));
        }
        // Fifth request at same instant must be rejected (window full).
        assert(!limiter.allow(t));
        // After 0.6 seconds, earliest request expires from window.
        t += 0.6;
        assert(limiter.allow(t)); // now allowed because window has space.
    }

    // Test 4: Monotonic time enforcement (debug assert).
    {
        SlidingWindowRateLimiter limiter(2, 2.0, 1.0);
        double t = 1.0;
        assert(limiter.allow(t));
        // The following call with earlier time would trigger an assert in debug builds.
        // We skip it in release mode; here we just ensure no crash for correct usage.
        t = 2.0;
        assert(limiter.allow(t));
    }

    // Test 5: Edge case – very small window, high rate.
    {
        SlidingWindowRateLimiter limiter(1, 1000.0, 0.001); // essentially a single‑shot limiter
        double t = 0.0;
        assert(limiter.allow(t));
        // Immediate second request should be rejected.
        assert(!limiter.allow(t));
        // After 0.002 seconds, window expires and token refilled.
        t += 0.002;
        assert(limiter.allow(t));
    }

    std::cout << "All unit tests passed.\n";
    return 0;
}
