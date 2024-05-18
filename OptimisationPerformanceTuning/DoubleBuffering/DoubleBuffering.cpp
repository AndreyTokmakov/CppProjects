/**============================================================================
Name        : DoubleBuffering.cpp
Created on  : 16.05.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DoubleBuffering.cpp
============================================================================**/

#include "DoubleBuffering.h"
#include "../Utilities.h"

#include <random>
#include <semaphore>
#include <span>
#include <thread>
#include <vector>

namespace DoubleBuffering
{
    // Function for generating data
    void generate_data(std::span<int> data)
    {
        // Create random number generator
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution dist(1, 100);

        // Generate random data
        for (auto &value : data) {
            value = dist(mt);
        }
    }

    // Function for processing data (dummy work)
    void process_data(std::span<int> data)
    {
        for (int i = 0; i < 5; i++) {
            for (auto &value : data) {
                value %= value + 1;
            }
        }
    }

    // Number of iterations to perform
    constexpr uint32_t numIterations { 100 };
    constexpr uint32_t size { 1048576 };

    void Baseline()
    {
        // Create std::vector passed between generation/processing
        std::vector<int> data(size);

        // Go through 100 iterations of generating/processing data
        for (uint32_t i = 0; i < numIterations; i++) {
            generate_data(data);
            process_data(data);
        }
    }

    void WithDoubleBuffering()
    {
        // Create std::vector passed between generation/processing
        std::vector<int> data_1 (size);
        std::vector<int> data_2 (size);

        // Semaphores for coordinating work
        std::binary_semaphore signal_to_process {0};
        std::binary_semaphore signal_to_generate {1};

        // Work function for generating data
        auto data_generator = [&]() {
            for (uint32_t i = 0; i < numIterations; i++) {
                generate_data(data_1);   // Generate data
                signal_to_generate.acquire(); // Wait until current processing work is done
                data_1.swap(data_2);       // Swap the vectors
                signal_to_process.release();  // Signal the processing thread to begin
            }
        };

        // Work function for generating data
        auto data_processor = [&]() {
            for (uint32_t i = 0; i < numIterations; i++) {
                signal_to_process.acquire();   // Wait until the current data generation is done
                process_data(data_2);     // Process the data
                signal_to_generate.release();  // Signal the processing thread to begin
            }
        };

        // Spawn our two threads
        std::jthread generator(data_generator);
        std::jthread processor(data_processor);
    }

}


void DoubleBuffering::TestAll()
{
    {
        Utilities::ScopedTimer timer {"Baseline"};
        Baseline();
    }
    {
        Utilities::ScopedTimer timer {"WithDoubleBuffering"};
        WithDoubleBuffering();
    }
};