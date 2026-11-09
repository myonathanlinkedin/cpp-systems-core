#include <iostream>
#include <cassert>
#include "types.hpp"
#include "engine.cpp"   // Include implementation directly for a single‑translation‑unit build

using namespace ratelimit;

int main() {
    // Test 1: Basic token bucket allowance
    {
        RateLimiterConfig cfg(5.0, 10.0, 1.0, 5); // 5 tokens/sec, capacity 10, 1‑sec window, max 5 req
        SlidingWindowRateLimiter limiter(cfg);
        double t = 0.0;
        // First 5 requests should pass
        for (int i = 0; i < 5; ++i) {
            assert(limiter.allow(t) && "Initial request should be allowed");
            t += 0.1; // 0.1 s between requests
        }
        // 6th request within same window exceeds max_requests (5)
        assert(!limiter.allow(t) && "Exceeding max_requests should be denied");
    }

    // Test 2: Token refill after idle period
    {
        RateLimiterConfig cfg(2.0, 4.0, 2.0, 4); // 2 tokens/sec, capacity 4, 2‑sec window
        SlidingWindowRateLimiter limiter(cfg);
        double t = 0.0;
        // Consume all tokens
        for (int i = 0; i < 4; ++i) {
            assert(limiter.allow(t));
            t += 0.1;
        }
        // No tokens left; next request denied
        assert(!limiter.allow(t));
        // Advance time by 2 seconds → should refill to capacity (4 tokens)
        t += 2.0;
        assert(limiter.allow(t) && "Tokens should have refilled after idle period");
    }

    // Test 3: Sliding window eviction
    {
        RateLimiterConfig cfg(10.0, 10.0, 1.0, 10); // 10 req/sec limit
        SlidingWindowRateLimiter limiter(cfg);
        double t = 0.0;
        // Issue 10 requests within 0.9 s
        for (int i = 0; i < 10; ++i) {
            assert(limiter.allow(t));
            t += 0.09;
        }
        // Next request should be denied (window full)
        assert(!limiter.allow(t));
        // Advance time just beyond the oldest request (0.91 s later)
        t += 0.02; // now oldest request is >1 s ago
        assert(limiter.allow(t) && "Old request should have expired from sliding window");
    }

    // Test 4: Merge two limiters (distributed scenario)
    {
        RateLimiterConfig cfg(5.0, 5.0, 1.0, 5);
        SlidingWindowRateLimiter a(cfg);
        SlidingWindowRateLimiter b(cfg);
        double t = 0.0;
        // a consumes 3 tokens
        for (int i = 0; i < 3; ++i) {
            assert(a.allow(t));
            t += 0.1;
        }
        // b consumes 2 tokens
        t = 0.0;
        for (int i = 0; i < 2; ++i) {
            assert(b.allow(t));
            t += 0.1;
        }
        // Merge b into a; a should now have min(2,2)=2 tokens left (since both had 2 tokens left)
        a.merge(b);
        // a should allow exactly 2 more requests before token exhaustion
        assert(a.allow(1.0));
        assert(a.allow(1.1));
        assert(!a.allow(1.2));
    }

    // Test 5: Edge case – zero rate
    {
        RateLimiterConfig cfg(0.0, 0.0, 1.0, 0);
        SlidingWindowRateLimiter limiter(cfg);
        assert(!limiter.allow(0.0) && "Zero rate should never allow a request");
    }

    std::cout << "All unit tests passed.\n";
    return 0;
}
