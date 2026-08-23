/**============================================================================
Name        : SpdLogWrapper.cpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpdLogWrapper.cpp
============================================================================**/

#include "SpdLogWrapper.hpp"

#include "logger_factory.hpp"


void spd_log_wrapper::TestAll()
{
    const auto logger = spd_log_wrapper::createLogger();

    for (int i = 0; i < 10'000; i++) {
        logger->info("Message-" + std::to_string(i));
    }
}
