/**============================================================================
Name        : RateLimiter.cpp
Created on  : 27.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RateLimiter.cpp
============================================================================**/

#include "RateLimiter.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>

namespace rate_limiter
{
    class RateLimiter
    {
        struct UserQueue
        {
            std::vector<uint64_t> buffer;
            size_t head = 0;
            size_t tail = 0;
            size_t size= 0;

            explicit UserQueue(const size_t capacity): buffer (capacity){
            }

            bool empty() const {
                return size == 0;
            }

            bool full() const {
                return size == buffer.size();
            }

            uint64_t front() const {
                return buffer[head];
            }

            void pop() {
                head = (head + 1) % buffer.size();
                --size;
            }

            void push(const uint64_t timestamp)
            {
                buffer[tail] = timestamp;
                tail = (tail+ 1) % buffer.size();
                ++size;
            }
        };

        size_t limit {0};
        uint64_t windowSeconds { 0 };
        std::unordered_map<std::string, UserQueue> users;

    public:

        RateLimiter(const size_t limit,const  uint64_t windowSeconds):
            limit (limit), windowSeconds (windowSeconds)
        {
            if (windowSeconds == 0) {
                throw std::invalid_argument("windowSeconds must be greater than zero");
            }
        }

        bool allow (const std::string& userId, const uint64_t timestampSeconds)
        {
            // Semantics: no requests are permitted.
            if (limit == 0) {
                return false;
            }

            UserQueue& queue = users.try_emplace(userId, limit).first->second;

            // Active window: [timestampSeconds - windowSeconds + 1, timestampSeconds].
            while (!queue.empty())
            {
                const uint64_t oldest = queue.front();
                // Timestamps are expected in nondecreasing order.
                if (timestampSeconds < oldest || timestampSeconds - oldest < windowSeconds) {
                    break;
                }
                queue.pop();
            }

            if (queue.full()) {
                return false;
            }

            queue.push(timestampSeconds);
            return true;
        }
    };

    class TokenBucketRateLimiter
    {
        struct Bucket
        {
            double tokens;
            uint64_t lastRefillTimestampMs;

            Bucket(const double initialTokens, const uint64_t timestampMs):
                tokens { initialTokens }, lastRefillTimestampMs { timestampMs }
            {
            }
        };

        double capacity { 0.0 };
        double refillRatePerSecond { 0.0 };
        std::unordered_map<std::string, Bucket> buckets;

    public:

        // capacity: maximum burst size
        // refillRatePerSecond: tokens added per second
        TokenBucketRateLimiter (const double capacity, const double refillRatePerSecond):
            capacity { capacity }, refillRatePerSecond { refillRatePerSecond }
        {
        }

        // timestampMs is monotonic time in milliseconds.
        bool allow(const std::string& userId, uint64_t timestampMs)
        {
            // Get the user's bucket or create a new one initialized with full capacity.
            // The timestamp is stored to calculate token refill on subsequent requests.
            Bucket& bucket = buckets.try_emplace(userId, capacity, timestampMs).first->second;

            // Calculate the time elapsed since the last token refill.
            const uint64_t elapsedMs = timestampMs - bucket.lastRefillTimestampMs;

            // Calculate how many tokens have accumulated during the elapsed time.
            // refillRatePerSecond defines the number of tokens added per second.
            const double tokensToAdd = static_cast<double>(elapsedMs) * refillRatePerSecond / 1000.0;

            // Refill the bucket, ensuring the number of tokens never exceeds capacity.
            bucket.tokens = std::min(capacity, bucket.tokens + tokensToAdd);

            // Update the timestamp to use as the starting point for the next refill.
            bucket.lastRefillTimestampMs = timestampMs;

            // A request requires one token.
            // If fewer than one token is available, reject the request.
            if (bucket.tokens < 1.0)
                return false;

            // Consume one token to allow the request.
            bucket.tokens -= 1.0;

            return true;
        }
    };
}


namespace
{
    using namespace std::chrono;
    using Clock = steady_clock;

    void assertTrue(const bool condition, const char* message)
    {
        if (!condition)
        {
            std::cerr << "FAIL: " << message << '\n';
            std::terminate();
        }
    }

    [[nodiscard]]
    uint64_t getTimestampMs() noexcept
    {
        return static_cast<uint64_t>(duration_cast<milliseconds>(Clock::now().time_since_epoch()).count());
    }

    void testTokenBucketRateLimiter()
    {
        // Capacity: 2 tokens.
        // Refill rate: 2 tokens per second.
        rate_limiter::TokenBucketRateLimiter limiter { 2.0, 2.0 };
        const std::string user { "user1" };

        // Initially the bucket contains two tokens.
        assertTrue(limiter.allow(user, getTimestampMs()), "First request should be allowed");
        assertTrue(limiter.allow(user, getTimestampMs()), "Second request should be allowed");

        // The bucket is empty.
        assertTrue(!limiter.allow(user, getTimestampMs()), "Third request should be rejected");

        // Wait long enough to refill one token.
        std::this_thread::sleep_for(milliseconds { 600 });

        assertTrue(limiter.allow(user, getTimestampMs()), "Request should be allowed after refill");

        // The consumed token should leave the bucket empty.
        assertTrue(!limiter.allow(user, getTimestampMs()), "Next request should be rejected");

        // Wait long enough to refill the bucket to its capacity.
        std::this_thread::sleep_for(seconds { 2 });

        assertTrue(limiter.allow(user, getTimestampMs()), "Request should be allowed after full refill");
        assertTrue(limiter.allow(user, getTimestampMs()), "Second request should be allowed after full refill");
        assertTrue(!limiter.allow(user, getTimestampMs()), "Request should be rejected after consuming all tokens");
    }
}

void rate_limiter::TestAll()
{
    testTokenBucketRateLimiter();

}
