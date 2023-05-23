//============================================================================
// Name        : SynchStream.h
// Created on  : 22.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : STP C++ SynchStream
//============================================================================

#include "SynchStream.h"

#include <iostream>
#include <algorithm>
#include <string_view>
#include <vector>

#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

namespace SynchStream
{
    void NoSynchTest()
    {
        std::atomic<int> counter = 0;
        std::vector<std::future<void>> task;
        for (int i = 0; i < 5; ++i){
            task.emplace_back(std::async([&] {
                std::cout << "Counter value = " << ++counter << std::endl;
            }));
        }

        std::for_each(task.begin(), task.end(), [] (const auto& T){
            T.wait();
        });
    }

    void SynchTest_BAD()
    {
        std::atomic<int> counter = 0;
        std::vector<std::future<void>> task;
        std::osyncstream stream {std::cout};

        for (int i = 0; i < 5; ++i){
            task.emplace_back(std::async([&] {
                stream << "Counter value = " << ++counter << std::endl;
            }));
        }

        std::for_each(task.begin(), task.end(), [] (const auto& T){
            T.wait();
        });
    }

    void SynchTest()
    {
        std::atomic<int> counter = 0;
        std::vector<std::future<void>> task;

        for (int i = 0; i < 5; ++i){
            task.emplace_back(std::async([&] {
                std::osyncstream {std::cout} << "Counter value = " << ++counter << std::endl;
            }));
        }

        std::for_each(task.begin(), task.end(), [] (const auto& T){
            T.wait();
        });
    }
};

void SynchStream::TEST_ALL()
{
    // std::osyncstream(std::cout) << "Filling the resource and notifying the consumer.\n";

    // NoSynchTest();

    SynchTest_BAD();
    // SynchTest();

};