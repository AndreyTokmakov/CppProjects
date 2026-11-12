/**============================================================================
Name        : MovingAverage.cpp
Created on  : 12.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MovingAverage.cpp
============================================================================**/

#include "MovingAverage.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <print>
#include <iostream>
#include <source_location>

/*
 * Simple Moving Average (SMA) implemented using a circular buffer.
 *
 * The algorithm maintains a running sum of the last N values, where N is the
 * configured window size. When a new value is added, the oldest value is
 * subtracted from the running sum and replaced with the new value.
 *
 * The buffer index wraps around when the end of the buffer is reached.
 * This allows updating the moving average without iterating over the entire
 * window.
 *
 * Time Complexity:
 *   - Construction: O(N)
 *   - addPrice():   O(1)
 *   - getAverage(): O(1)
 *
 * Space Complexity: O(N)
 *
 * Each update performs a constant number of arithmetic and indexing operations,
 * making the algorithm suitable for processing streaming data.
 */

namespace
{
    struct  MovingAverage
    {
        explicit MovingAverage(const std::size_t size)
            : buffer(size, 0.0),maxSize(size)
        {
            if (size == 0) {
                throw std::invalid_argument("MovingAverage size must be greater than zero");
            }
        }

        void addPrice(const double price)
        {
            sum -= buffer[index];
            sum += price;

            buffer[index] = price;
            ++index;

            if (index == maxSize) {
                index = 0;
            }

            if (count < maxSize) {
                ++count;
            }
        }

        [[nodiscard]]
        double getAverage() const
        {
            return count == 0 ? 0.0 : sum / static_cast<double>(count);
        }

    private:
        std::vector<double> buffer;
        std::size_t maxSize { 0 };
        std::size_t index { 0 };
        std::size_t count  { 0 };
        double sum = 0.0;
    };
}

namespace
{
    void Assert(const bool condition,
                const std::source_location location = std::source_location::current())
    {
        if (!condition)
        {
            std::println(std::cerr, "\tFile: {}\n\tFunction: {}\n\tLine: {}",
                   location.file_name(), location.function_name(), location.line());
            std::terminate();
        }
    }

    void testEmptyWindow()
    {
        const MovingAverage average(3);
        Assert(average.getAverage() == 0.0);
    }

    void testSingleValue()
    {
        MovingAverage average(3);
        average.addPrice(10.0);
        Assert(average.getAverage() == 10.0);
    }

    void testPartialWindow()
    {
        MovingAverage average(3);

        average.addPrice(10.0);
        average.addPrice(20.0);

        Assert(average.getAverage() == 15.0);
    }

    void testFullWindow()
    {
        MovingAverage average(3);

        average.addPrice(10.0);
        average.addPrice(20.0);
        average.addPrice(30.0);

        Assert(average.getAverage() == 20.0);
    }

    void testWindowOverwrite()
    {
        MovingAverage average(3);

        average.addPrice(10.0);
        average.addPrice(20.0);
        average.addPrice(30.0);
        average.addPrice(40.0);

        // Window: [20, 30, 40]
        Assert(average.getAverage() == 30.0);
    }

    void testMultipleWindowOverwrites()
    {
        MovingAverage average(3);

        average.addPrice(10.0);
        average.addPrice(20.0);
        average.addPrice(30.0);
        average.addPrice(40.0);
        average.addPrice(50.0);
        average.addPrice(60.0);

        // Window: [40, 50, 60]
        Assert(average.getAverage() == 50.0);
    }

    void testWindowSizeOne()
    {
        MovingAverage average(1);

        average.addPrice(10.0);
        Assert(average.getAverage() == 10.0);

        average.addPrice(20.0);
        Assert(average.getAverage() == 20.0);

        average.addPrice(30.0);
        Assert(average.getAverage() == 30.0);
    }

    void testNegativeValues()
    {
        MovingAverage average(3);

        average.addPrice(-10.0);
        average.addPrice(20.0);
        average.addPrice(-30.0);

        Assert(std::abs(average.getAverage() - (-20.0 / 3.0)) < 1e-9);

        average.addPrice(40.0);

        // Window: [20, -30, 40]
        Assert(std::abs(average.getAverage() - 10.0) < 1e-9);
    }

    void testFractionalValues()
    {
        MovingAverage average(2);

        average.addPrice(1.5);
        average.addPrice(2.5);

        Assert(average.getAverage() == 2.0);

        average.addPrice(3.5);

        Assert(average.getAverage() == 3.0);
    }

    void testInvalidWindowSize()
    {
        bool exceptionThrown = false;

        try {
            MovingAverage average(0);
        }
        catch (const std::invalid_argument&) {
            exceptionThrown = true;
        }

        Assert(exceptionThrown);
    }
}


void moving_average::TestAll()
{
    testEmptyWindow();
    testSingleValue();
    testPartialWindow();
    testFullWindow();
    testWindowOverwrite();
    testMultipleWindowOverwrites();
    testWindowSizeOne();
    testNegativeValues();
    testFractionalValues();
    testInvalidWindowSize();
}
