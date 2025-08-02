/**============================================================================
Name        : Logger.cpp
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Logger.cpp
============================================================================**/

#include "Logger.hpp"

namespace logger
{
    Logger::Logger(std::string loggerName, const std::filesystem::path& logFilePath):
            stdoutSink { std::make_shared<stdout_color_sink_mt>() },
            rotatingSink {
                std::make_shared<rotating_file_sink_mt>(logFilePath, logMaxSize, logMaxFiles)
            }
    {
        spdlog::init_thread_pool(queueSize, threadCount);

        const std::vector<spdlog::sink_ptr> sinks { stdoutSink, rotatingSink };
        logger = std::make_shared<spdlog::async_logger>(std::move(loggerName),
                sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        spdlog::register_logger(logger);
    }


}
