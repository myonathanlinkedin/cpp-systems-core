#include <deque>
#include <algorithm>

#include "types.hpp"

namespace ratelimit {

SlidingWindowRateLimiter::SlidingWindowRateLimiter(const RateLimiterConfig& cfg)
    : config_(cfg),
      last_timestamp_(0.0),
      available_tokens_(cfg.max_tokens) // start full
{
    // Ensure configuration is sensible
    if (config_.tokens_per_second < 0.0) config_.tokens_per_second = 0.0;
    if (config_.max_tokens < 0.0) config_.max_tokens = 0.0;
    if (config_.window_seconds < 0.0) config_.window_seconds = 0.0;
    // max_requests defaults to floor(tokens_per_second * window_seconds) if zero
    if (config_.max_requests == 0 && config_.tokens_per_second > 0.0 && config_.window_seconds > 0.0) {
        config_.max_requests = static_cast<std::size_t>(config_.tokens_per_second * config_.window_seconds);
    }
}

void SlidingWindowRateLimiter::purge_old_requests(double current_time) {
    while (!request_times_.empty() &&
           current_time - request_times_.front() > config_.window_seconds) {
        request_times_.pop_front();
    }
}

bool SlidingWindowRateLimiter::allow(double timestamp) {
    // Enforce monotonic timestamps
    if (timestamp < last_timestamp_) {
        return false;
    }

    // Refill tokens based on elapsed time
    double delta = timestamp - last_timestamp_;
    available_tokens_ = std::min(config_.max_tokens,
                                 available_tokens_ + delta * config_.tokens_per_second);
    last_timestamp_ = timestamp;

    // Clean up old requests for sliding window
    purge_old_requests(timestamp);

    // Enforce sliding window request count limit
    if (config_.max_requests > 0 && request_times_.size() >= config_.max_requests) {
        return false;
    }

    // Enforce token bucket limit (need at least one token per request)
    if (available_tokens_ < 1.0) {
        return false;
    }

    // Accept request
    available_tokens_ -= 1.0;
    request_times_.push_back(timestamp);
    return true;
}

void SlidingWindowRateLimiter::merge(const SlidingWindowRateLimiter& other) {
    // Merge token counts conservatively (minimum of the two)
    available_tokens_ = std::min(available_tokens_, other.available_tokens_);

    // Merge request timestamps, keep them sorted and within the window
    std::deque<double> merged;
    merged.insert(merged.end(), request_times_.begin(), request_times_.end());
    merged.insert(merged.end(), other.request_times_.begin(), other.request_times_.end());
    std::sort(merged.begin(), merged.end());

    // Remove duplicates and timestamps outside the window relative to the newer of the two last timestamps
    double reference_time = std::max(last_timestamp_, other.last_timestamp_);
    request_times_.clear();
    for (double t : merged) {
        if (reference_time - t <= config_.window_seconds) {
            // Avoid duplicate timestamps (unlikely but safe)
            if (request_times_.empty() || request_times_.back() != t) {
                request_times_.push_back(t);
            }
        }
    }

    // Update last_timestamp_ to the newer one
    last_timestamp_ = reference_time;
}

} // namespace ratelimit
