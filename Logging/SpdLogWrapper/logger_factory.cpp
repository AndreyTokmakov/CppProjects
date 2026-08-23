/**============================================================================
Name        : logger_factory.cpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Factory for application logging infrastructure.
============================================================================**/

#include "logger_factory.hpp"
#include "spdlog_logger.hpp"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace spd_log_wrapper
{
    std::shared_ptr<ILogger> createLogger(const LoggerConfiguration& configuration)
    {
        spdlog::init_thread_pool(configuration.queueSize, configuration.workerThreads);

        const auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            configuration.fileName,
            configuration.maxFileSize,
            configuration.maxFiles
        );

        const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks {
            consoleSink,
            fileSink
        };

        auto logger = std::make_shared<spdlog::async_logger>(
            configuration.name,
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::overrun_oldest
        );

        logger->set_level(spdlog::level::info);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        spdlog::register_logger(logger);
        return std::make_shared<SpdlogLogger>(std::move(logger));
    }
}