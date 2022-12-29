//============================================================================
// Name        : ParallelExecutionAlgoritms.h
// Created on  : 14.01.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Parallel execution algoritms src
//============================================================================

#include "ParallelExecutionAlgoritms.h"

#include <iostream>
#include <string>

#include <thread>
#include <mutex>
#include <chrono>

#include <algorithm>
#include <execution>

#include <array>
#include <vector>
#include <unordered_set>

namespace ParallelExecutionAlgoritms {


    /*
    void SimpleTest_ParUnseq() {
        int x = 0;
        std::mutex m;
        std::vector<int> data(1000);
        std::iota(data.begin(), data.end(), 0);
        std::unordered_set<std::thread::id> threads;

        std::for_each(std::execution::par_unseq, data.cbegin(), data.cend(), [&](int) {
            std::lock_guard<std::mutex> guard(m);
            ++x;

            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            threads.insert(std::this_thread::get_id());
        });

        std::cout << "Task performed by " << threads.size() << " different threads:\n";
        for (const auto& id: threads)
            std::cout << "   " << id << std::endl;
    }


    void SimpleTest_Unseq() {
        int x = 0;
        std::mutex m;
        std::vector<int> data(1000);
        std::iota(data.begin(), data.end(), 0);
        std::unordered_set<std::thread::id> threads;

        std::for_each(std::execution::unseq, data.cbegin(), data.cend(), [&](int) {
            std::lock_guard<std::mutex> guard(m);
            ++x;

            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            threads.insert(std::this_thread::get_id());
        });

        std::cout << "Task performed by " << threads.size() << " different threads:\n";
        for (const auto& id : threads)
            std::cout << "   " << id << std::endl;
    }

    void SimpleTest_Par() {
        int x = 0;
        std::mutex m;
        std::vector<int> data(1000);
        std::iota(data.begin(), data.end(), 0);
        std::unordered_set<std::thread::id> threads;

        std::for_each(std::execution::par, data.cbegin(), data.cend(), [&](int) {
            std::lock_guard<std::mutex> guard(m);
            ++x;

            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            threads.insert(std::this_thread::get_id());
        });

        std::cout << "Task performed by " << threads.size() << " different threads:\n";
        for (const auto& id : threads)
            std::cout << "   " << id << std::endl;
    }

    // execution may not be parallelized
    void SimpleTest_Seq() {
        int x = 0;
        std::mutex m;
        std::vector<int> data(1000);
        std::iota(data.begin(), data.end(), 0);
        std::unordered_set<std::thread::id> threads;

        std::for_each(std::execution::seq, data.cbegin(), data.cend(), [&](int) {
            std::lock_guard<std::mutex> guard(m);
            ++x;

            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            threads.insert(std::this_thread::get_id());
        });

        std::cout << "Task performed by " << threads.size() << " different threads:\n";
        for (const auto& id : threads)
            std::cout << "   " << id << std::endl;
    }

    static void Parallel_Execution_Test() {
        std::vector<int> vec = { 3, 2, 1, 4, 5, 6, 10, 8, 9, 4 };

        //std::sort(vec.begin(), vec.end());                            // sequential as ever
        //std::sort(std::execution::seq, vec.begin(), vec.end());       // sequential
        //std::sort(std::execution::par, vec.begin(), vec.end());       // parallel
        // std::sort(std::execution::par_unseq, vec.begin(), vec.end()); // parallel and vectorized

        // std::for_each(vec.begin(), vec.end(), [](int v) {std::cout << v << std::endl; });
    }
    */
}

void ParallelExecutionAlgoritms::TEST_ALL() {

    // SimpleTest_Par();
    // SimpleTest_Seq();
    // SimpleTest_ParUnseq();
    // SimpleTest_Unseq();
}

