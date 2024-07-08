/**============================================================================
Name        : SPDLog.cpp
Created on  : 08.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SPDLog.cpp
============================================================================**/


#include <iostream>

#include "SPDLog.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/callback_sink.h"

namespace SPDLog
{
    void BasicTest()
    {
        spdlog::info("Welcome to spdlog!");
        spdlog::error("Some error message with arg: {}", 1);

        spdlog::warn("Easy padding in numbers like {:08d}", 12);
        spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        spdlog::info("Support for floats {:03.2f}", 1.23456);
        spdlog::info("Positional args are {1} {0}..", "too", "supported");
        spdlog::info("{:<30}", "left aligned");

        spdlog::set_level(spdlog::level::debug); // Set global log level to debug
        spdlog::debug("This message should be displayed..");

        // change log pattern
        spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

        // Compile time log levels
        // Note that this does not change the current log level, it will only
        // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
        SPDLOG_TRACE("Some trace message with param {}", 42);
        SPDLOG_DEBUG("Some debug message");
    }

    void Stdout_Stderr_Logger()
    {
        // create a color multi-threaded logger
        auto console = spdlog::stdout_color_mt("console");
        auto err_logger = spdlog::stderr_color_mt("stderr");

        spdlog::get("console")->info(
                "loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
    }

    void File_Logger()
    {
        try
        {
            std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("basic_logger", "/tmp/trace.log");
            logger->info("Info 1");
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cout << "Log init failed: " << ex.what() << std::endl;
        }
    }

    void Multi_Sink_Example()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::warn);
        console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("/tmp/trace1.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::logger logger("multi_sink", {console_sink, file_sink});
        logger.set_level(spdlog::level::debug);
        logger.warn("this should appear in both console and file");
        logger.info("this message should not appear in the console, only in the file");
    }

    void Callback_Example()
    {
        auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg &msg) {
             // for example you can be notified by sending an email to yourself
                 std::cout << "SINK  " << msg.payload.data() << "\n";
        });
        callback_sink->set_level(spdlog::level::err);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        spdlog::logger logger("custom_callback_logger", {console_sink, callback_sink});

        logger.info("some info log");
        logger.error("critical issue"); // will notify you
    }
}


/** https://github.com/gabime/spdlog **/

void SPDLog::TestAll()
{

    // SPDLog::BasicTest();
    // SPDLog::Stdout_Stderr_Logger();
    // SPDLog::File_Logger();
    // SPDLog::Multi_Sink_Example();
    SPDLog::Callback_Example();

}
