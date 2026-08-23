/**============================================================================
Name        : spdlog_logger.cpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Asynchronous spdlog-based logger implementation.
============================================================================**/

#include "spdlog_logger.hpp"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace spd_log_wrapper
{
    SpdlogLogger::SpdlogLogger(std::shared_ptr<spdlog::logger> logger) noexcept:
        logger { std::move(logger) }
    {
    }

    void SpdlogLogger::trace(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::trace, "{}", message);
    }

    void SpdlogLogger::debug(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::debug, "{}", message);
    }

    void SpdlogLogger::info(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::info, "{}", message);
    }

    void SpdlogLogger::warn(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::warn, "{}", message);
    }

    void SpdlogLogger::error(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::err, "{}", message);
    }

    void SpdlogLogger::critical(const std::string_view message) noexcept
    {
        logger->log(spdlog::level::critical, "{}", message);
    }
}