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
#include "spdlog/async.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace
{
    const std::string logsDir { R"(/home/andtokm/DiskS/Temp/Logs)"};
}

namespace SPDLog::Basics
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

    void File_Logger_Async()
    {
        try
        {
            std::shared_ptr<spdlog::logger> async_file_logger =
                    spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "/tmp/trace.txt");

            for (int i = 1; i < 101; ++i) {
                async_file_logger->info("Async message #{}", i);
            }

            // Under VisualStudio, this must be called before main finishes to workaround a known VS issue
            spdlog::drop_all();
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cout << "Log init failed: " << ex.what() << std::endl;
        }
    }

    void Multi_Sink_Example()
    {
        const std::shared_ptr<spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_mutex>> console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        console_sink->set_level(spdlog::level::warn);
        console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        const std::shared_ptr<spdlog::sinks::basic_file_sink<std::mutex>> file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>("/tmp/trace.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::logger logger("multi_sink", {console_sink, file_sink});
        logger.set_level(spdlog::level::debug);
        logger.warn("this should appear in both console and file");
        logger.info("this message should not appear in the console, only in the file");
    }

    void Callback_Example()
    {
        const std::shared_ptr<spdlog::sinks::callback_sink<std::mutex>> callbackSink {
            std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg &msg) {
                 std::cout << "SINK  " << msg.payload.data() << "\n";
        })};
        const std::shared_ptr<spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_mutex>> consoleSink {
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        };

        spdlog::logger logger("custom_callback_logger", { consoleSink, callbackSink });

        logger.info("some info log");
        logger.error("critical issue"); // will notify you
    }
}

namespace SPDLog::Async
{
    void AsyncLogger_PoolSettings()
    {
        try
        {
            const std::shared_ptr<spdlog::sinks::daily_file_sink<std::mutex>> daily_sink =
                    std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", 23, 59);

            // default thread pool settings can be modified *before* creating the async logger:
            spdlog::init_thread_pool(10000, 1); // queue with 10K items and 1 backing thread.

            const std::shared_ptr<spdlog::logger> asyncFileLogger = spdlog::basic_logger_mt<spdlog::async_factory>(
                    "async_file_logger", "/tmp/trace.txt");

            for (int i = 1; i < 101; ++i) {
                asyncFileLogger->info("Async message #{}", i);
            }

            spdlog::drop_all();
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    void AsynchronousLogger_MultiSinks()
    {
        spdlog::init_thread_pool(8192, 1);

        const std::shared_ptr<spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_mutex>> stdoutSink {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        };
        const std::shared_ptr<spdlog::sinks::rotating_file_sink<std::mutex>> rotatingSink {
                std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logsDir + "/trace.txt",
                                                                       1024 * 1024, 10)
        };

        std::vector<spdlog::sink_ptr> sinks { stdoutSink, rotatingSink };
        const std::shared_ptr<spdlog::async_logger>  logger = std::make_shared<spdlog::async_logger>(
            "logger_name", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

        spdlog::register_logger(logger);

        for (int i = 1; i < 100'000; ++i) {
            logger->info("Async message #{}", i);
        }

        // Will create a 8 log file [trace.txt, trace.1.txt .... trace.7.txt]
    }
}


// https://github.com/gabime/spdlog

void SPDLog::TestAll()
{
    // Basics::BasicTest();
    // Basics::Stdout_Stderr_Logger();

    // Basics::File_Logger();
    // Basics::File_Logger_Async();

    // Basics::Multi_Sink_Example();

    // Basics::Callback_Example();


    // Async::AsyncLogger_PoolSettings();
    Async::AsynchronousLogger_MultiSinks();
}
