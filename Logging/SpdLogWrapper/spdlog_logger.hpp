/**============================================================================
Name        : spdlog_logger.hpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Asynchronous spdlog-based logger implementation.
============================================================================**/

#ifndef FINANCETECHNOLOGYPROJECTS_SPDLOG_LOGGER_HPP
#define FINANCETECHNOLOGYPROJECTS_SPDLOG_LOGGER_HPP

#include "logger.hpp"

#include <memory>
#include <string_view>

namespace spdlog
{
    class logger;
}

namespace spd_log_wrapper
{
    class SpdlogLogger final : public ILogger
    {
    public:
        explicit SpdlogLogger(std::shared_ptr<spdlog::logger> logger) noexcept;

        void trace(std::string_view message) noexcept override;
        void debug(std::string_view message) noexcept override;
        void info(std::string_view message) noexcept override;
        void warn(std::string_view message) noexcept override;
        void error(std::string_view message) noexcept override;
        void critical(std::string_view message) noexcept override;

    private:
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // FINANCETECHNOLOGYPROJECTS_SPDLOG_LOGGER_HPP