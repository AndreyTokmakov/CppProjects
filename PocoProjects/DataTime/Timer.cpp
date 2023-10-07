//============================================================================
// Name        : Timer.h
// Created on  : 25.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Timer
//============================================================================

#include "Timer.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/Stopwatch.h"
#include <iostream>

namespace Timer
{
    class PocoTimer {
    private:
        Poco::Stopwatch stopWatch;

    public:
        PocoTimer() {
            stopWatch.start();
        }
        void PrintElapsedTime([[maybe_unused]] Poco::Timer& timer) {
            std::cout << "Time elapsed: " << stopWatch.elapsed() / 1000 << " milliseconds." << std::endl;
        }
    };

    void startTimer() {
        PocoTimer pocoTimer;
        Poco::Timer timer(250, 500);
        timer.start(Poco::TimerCallback<PocoTimer>(pocoTimer, &PocoTimer::PrintElapsedTime));
        Poco::Thread::sleep(5000);
        timer.stop();
    }

}

void Timer::TestAll() {
    startTimer();
}

