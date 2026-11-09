#pragma once

#include <deque>
#include <algorithm>
#include <cstddef>

namespace ratelimit {

struct RateLimiterConfig {
    double tokens_per_second = 0.0;   // Rate at which tokens are added
    double max_tokens = 0.0;          // Maximum token bucket capacity
    double window_seconds = 0.0;      // Sliding window duration
    std::size_t max_requests = 0;     // Maximum allowed requests in a window

    RateLimiterConfig() = default;
    RateLimiterConfig(double tps, double maxTok, double winSec, std::size_t maxReq)
        : tokens_per_second(tps), max_tokens(maxTok), window_seconds(winSec), max_requests(maxReq) {};
};

class SlidingWindowRateLimiter {
public:
    explicit SlidingWindowRateLimiter(const RateLimiterConfig& cfg);

    // Returns true if a request at the given monotonic timestamp (seconds) is allowed.
    bool allow(double timestamp);

    // Merge the state of another limiter into this one (useful for distributed scenarios).
    // The merge conservatively keeps the minimum available tokens and the union of request timestamps.
    void merge(const SlidingWindowRateLimiter& other);

    // Accessors for testing
    double available_tokens() const noexcept { return available_tokens_; };
    double last_timestamp() const noexcept { return last_timestamp_; }
    const std::deque<double>& request_times() const noexcept { return request_times_; }

private:
    RateLimiterConfig config_;
    double last_timestamp_;                 // Last time the bucket was updated
    double available_tokens_;               // Current token count
    std::deque<double> request_times_;      // Timestamps of requests inside the sliding window

    // Helper to purge timestamps older than the sliding window.
    void purge_old_requests(double current_time);
};

} // namespace ratelimit
