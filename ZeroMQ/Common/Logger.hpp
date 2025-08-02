/**============================================================================
Name        : Logger.h
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Logger.h
============================================================================**/

#ifndef CPPWORKPROJECTS_LOGGER_H
#define CPPWORKPROJECTS_LOGGER_H

#include <string_view>
#include <vector>
#include <filesystem>
#include <utility>

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace logger
{
    using namespace spdlog::sinks;
    using namespace spdlog::details;

    struct Logger
    {
        constexpr static inline std::size_t logMaxSize { 1024 * 1024 };
        constexpr static inline std::size_t logMaxFiles { 10 };
        constexpr static inline std::size_t queueSize { 8192 };
        constexpr static inline std::size_t threadCount { 1 };

        std::shared_ptr<ansicolor_stdout_sink<console_mutex>> stdoutSink { nullptr };
        std::shared_ptr<spdlog::sinks::rotating_file_sink<std::mutex>> rotatingSink { nullptr };
        std::shared_ptr<spdlog::async_logger> logger { nullptr };

        Logger(std::string loggerName, const std::filesystem::path& logFilePath = {"/tmp/Logs/trace.log"});

        template <typename... Args>
        void trace(Args&&... args) {
            logger->trace( std::forward<Args>(args) ...);
        }

        template <typename... Args>
        void debug(Args&&... args) {
            logger->debug( std::forward<Args>(args) ...);
        }

        template <typename... Args>
        void info(Args &&... params) {
            logger->info(std::forward<Args>(params) ...);
        }

        template <typename... Args>
        void warn(Args&&... args) {
            logger->warn( std::forward<Args>(args) ...);
        }

        template <typename... Args>
        void error(Args&&... args) {
            logger->error( std::forward<Args>(args) ...);
        }

        template <typename... Args>
        void critical(Args&&... args) {
            logger->critical( std::forward<Args>(args) ...);
        }
    };
}

#endif //CPPWORKPROJECTS_LOGGER_H
