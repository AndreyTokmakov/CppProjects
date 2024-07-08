/**============================================================================
Name        : Quill.cpp
Created on  : 08.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Quill.cpp
============================================================================**/

#include "Quill.h"

#include "quill/Quill.h"


#define MY_LOG_INFO(fmt, ...) QUILL_LOG_INFO(quill::get_root_logger(), fmt, ##__VA_ARGS__)


namespace Quill
{

    void TestOne()
    {
        // Start the logging backend thread
        quill::start();

        MY_LOG_INFO("Hello quill!");


        {
            quill::Logger* logger = quill::get_logger();

            // Change the LogLevel to print everything
            logger->set_log_level(quill::LogLevel::TraceL3);

            LOG_TRACE_L3(logger, "This is a log trace l3 example {}", 1);
            LOG_TRACE_L2(logger, "This is a log trace l2 example {} {}", 2, 2.3);
            LOG_TRACE_L1(logger, "This is a log trace l1 {} example", "string");
            LOG_DEBUG(logger, "This is a log debug example {}", 4);
            LOG_INFO(logger, "This is a log info example {}", 5);
            LOG_WARNING(logger, "This is a log warning example {}", 6);
            LOG_ERROR(logger, "This is a log error example {}", 7);
            LOG_CRITICAL(logger, "This is a log critical example {}", 8);

        }
    }
}

void Quill::TestAll()
{
    TestOne();
}