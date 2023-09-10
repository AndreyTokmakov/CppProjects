//============================================================================
// Name        : Timer.h
// Created on  : 21.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Timer
//============================================================================

#include "Timer.h"
#include "../common.h"

#include <iostream>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

namespace Timer {

    struct EventData final {
        int myData {0};
    };

    void expired(sigval timer_data) {
        EventData *data = static_cast<EventData *>(timer_data.sival_ptr);
        std::cout << "Timer " << (++data->myData) << " fired. Thread id: " <<  gettid() << std::endl;
    }

    void StartTimer() {
        EventData eventData { };

        /* sigevent specifies behaviour on expiration  */
        sigevent sev = { 0 , 0, 0, 0};

        /* specify start delay and interval it_value and it_interval must not be zero */
        itimerspec its {};
        its.it_value.tv_sec = 1;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 1;
        its.it_interval.tv_nsec = 0;

        std::cout << "Simple Threading Timer - thread-id: " << gettid() << std::endl;
        // std::cout << "Simple Threading Timer - thread-id: " << std::this_thread::get_id() << std::endl;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_notify_function = &expired;
        sev.sigev_value.sival_ptr = &eventData;


        /* create timer */
        timer_t timerId { };
        int res = timer_create(CLOCK_REALTIME, &sev, &timerId);
        if (0 != res) {
            std::cout << "timer_create() failed. Error = " << errno << std::endl;
            return;
        }

        /* start timer */
        res = timer_settime(timerId, 0, &its, nullptr);

        std::this_thread::sleep_for(std::chrono::seconds (10));
    }
};

void Timer::TestAll()
{
    StartTimer();
}