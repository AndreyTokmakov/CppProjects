/**============================================================================
Name        : Timer.h
Created on  : 03.09.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Timer
============================================================================**/

#include "Timer.h"

#include <iostream>
#include <string_view>

#include <thread>
#include <chrono>

#include <functional>

#include <boost/timer/timer.hpp>

using namespace boost::timer;

namespace Timer {
};

void Timer::TestAll()
{
    cpu_timer timer;

    std::cout << timer.format() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(2));
    timer.stop();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << timer.format() << '\n';

    timer.resume();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << timer.format() << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(100));

};