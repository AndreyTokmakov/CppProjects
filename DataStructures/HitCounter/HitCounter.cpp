/**============================================================================
Name        : HitCounter.cpp
Created on  : 27.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HitCounter.cpp
============================================================================**/

#include "HitCounter.hpp"
#include <queue>

namespace
{
    struct HitCounter
    {
        using timestamp_t = uint32_t;
        using counter_t  = uint32_t;

        void hit(timestamp_t timestamp)
        {
            // Fast path: another request within the same second.
            if (!hits.empty() && hits.back().first == timestamp)
            {
                ++hits.back().second;
                ++totalHits;
                return;
            }

            // Time advanced: expire outdated buckets.
            removeOldHits(timestamp);
            hits.emplace(timestamp, totalHits);
            ++totalHits;
        }

        uint32_t getHits(const timestamp_t timestamp)
        {
            removeOldHits (timestamp);
            return totalHits;
        }

    private:

        constexpr static timestamp_t WindowSize { 300 };

        std::queue<std::pair<timestamp_t, counter_t>> hits;
        counter_t totalHits = 0;

        void removeOldHits(const timestamp_t timestamp)
        {
            while (!hits.empty() && hits.front().first <= timestamp - WindowSize) {
                totalHits -= hits.front().second;
                hits.pop();
            }
        }
    };

    class HitCounter_RingBuffer
    {
    private:
        using timestamp_t = uint32_t;
        using counter_t  = uint32_t;

        constexpr static counter_t WindowSize { 300 };

        // { timestamp, number_of_hits_at_this_timestamp )
        std::array<std::pair<timestamp_t, counter_t>, WindowSize> slots;

    public:

        void hit (const timestamp_t timestamp)
        {
            const counter_t index = timestamp % WindowSize;
            if (auto& [storedTimestamp, count] = slots [index]; storedTimestamp != timestamp)
            {
                storedTimestamp = timestamp;
                count = 1;
            } else {
                ++count;
            }
        }

        counter_t getHits (const timestamp_t timestamp) const
        {
            counter_t total = 0;
            for (const auto& [storedTimestamp, count]: slots)
            {
                // Valid window: [timestamp 299, timestamp]
                if (timestamp - storedTimestamp < WindowSize) {
                    total += count;
                }
            }
            return total;
        }
    };
}

void hit_counter::TestAll()
{

}
