//============================================================================
// Name        : Time.cpp
// Created on  : 21.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Time
//============================================================================

#include "Time.h"
#include "../common.h"

#include <iostream>
#include <thread>

namespace Time {

    void Measure_Duration() {

        timeval start {}, end {};
        gettimeofday(&start, nullptr);

        // usleep(2000);
        std::this_thread::sleep_for(std::chrono::milliseconds (1500));

        gettimeofday(&end, nullptr);
        const long seconds  = end.tv_sec  - start.tv_sec,
                   useconds = end.tv_usec - start.tv_usec;
        long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        std::cout <<"Elapsed time: " << mtime << " milliseconds\n";
    }

    void Countdown_TIME() {
        time_t the_time;
        for (auto i = 1; i <= 5; ++i) {
            the_time = time(nullptr);
            std::cout <<"The time is: " << the_time << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds (1));
        }
    }


    void GetCurrentTime_AsString() {
        auto t = time (nullptr);
        std::cout << "Time is: " << ctime (&t) << std::endl;
    }

    void GetCurrentTime_Asctime() {
        const time_t now { time(nullptr) };
        const tm *time = gmtime(&now);
        const std::string_view currentTime {asctime(time) };

        std::cout << currentTime << std::endl;
    }
};

void Time::TestAll()
{
    // Measure_Duration();

    // Countdown_TIME();

    // GetCurrentTime_AsString();
    GetCurrentTime_Asctime();

};