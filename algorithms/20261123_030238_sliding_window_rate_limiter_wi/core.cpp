#pragma once
#include <deque>
#include <algorithm>
#include <chrono>
#include <cassert>

/*
 * Sliding Window Rate Limiter with Distributed Token Bucket
 *
 * Guarantees:
 *  - O(1) amortized time per request (deque cleanup is linear in number of expired entries,
 *    but each entry is removed exactly once).
 *  - Thread‑unsafe (single‑threaded usage) – suitable for deterministic unit testing.
 *
 * Parameters:
 *   capacity   – maximum number of tokens that can be accumulated.
 *   refillRate – tokens added per second (may be fractional).
 *   windowSize – sliding window duration in seconds for burst limiting.
 *
 * The limiter combines a classic token bucket (for steady‑state rate) with a sliding
 * window (for burst control).  A request at time `now` is allowed iff:
 *     1. The bucket has at least one token after refilling.
 *     2. Fewer than `capacity` requests have been allowed in the interval
 *        (now - windowSize, now].
 *
 * After a successful request, one token is consumed and the request timestamp
 * is recorded in the sliding‑window deque.
 */
class SlidingWindowRateLimiter {
public:
    // Construct a limiter.
    //   capacity   – maximum burst size (also bucket capacity).
    //   refillRate – tokens per second added to the bucket.
    //   windowSize – length of the sliding window in seconds.
    SlidingWindowRateLimiter(double capacity,
                             double refillRate,
                             double windowSize)
        : capacity_(capacity),
          refillRate_(refillRate),
          windowSize_(windowSize),
          tokens_(capacity),
          lastRefillTime_(0.0) {
        assert(capacity_ > 0.0);
        assert(refillRate_ > 0.0);
        assert(windowSize_ > 0.0);
    };

    // Returns true if a request at time `now` (seconds) is permitted.
    // `now` must be non‑decreasing across calls.
    bool allow(double now) {
        assert(now >= lastRefillTime_); // monotonic time for deterministic behavior
        refill(now);
        expireOldEntries(now);
        if (tokens_ >= 1.0 && static_cast<std::size_t>(windowTimestamps_.size()) < static_cast<std::size_t>(capacity_)) {
            // Consume a token and record the request.
            tokens_ -= 1.0;
            windowTimestamps_.push_back(now);
            return true;
        }
        return false;
    }

    // For testing: expose internal state (read‑only).
    double tokens() const { return tokens_; }
    std::size_t windowSize() const { return windowTimestamps_.size(); }

private:
    const double capacity_;
    const double refillRate_;
    const double windowSize_;

    double tokens_;               // current token count (may be fractional)
    double lastRefillTime_;       // last time the bucket was refilled
    std::deque<double> windowTimestamps_; // timestamps of allowed requests within the window

    // Refill the token bucket based on elapsed time.
    void refill(double now) {
        double elapsed = now - lastRefillTime_;
        if (elapsed > 0.0) {
            tokens_ = std::min(capacity_, tokens_ + elapsed * refillRate_);
            lastRefillTime_ = now;
        }
    }

    // Remove timestamps that fall outside the sliding window.
    void expireOldEntries(double now) {
        double cutoff = now - windowSize_;
        while (!windowTimestamps_.empty() && windowTimestamps_.front() <= cutoff) {
            windowTimestamps_.pop_front();
        }
    }
};
