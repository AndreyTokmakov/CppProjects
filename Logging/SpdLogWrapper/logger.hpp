/**============================================================================
Name        : logger.hpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Logging interface used by the trading system.
============================================================================**/

#ifndef FINANCETECHNOLOGYPROJECTS_LOGGER_HPP
#define FINANCETECHNOLOGYPROJECTS_LOGGER_HPP

#include <string_view>

namespace spd_log_wrapper
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        virtual void trace(std::string_view message) noexcept = 0;
        virtual void debug(std::string_view message) noexcept = 0;
        virtual void info(std::string_view message) noexcept = 0;
        virtual void warn(std::string_view message) noexcept = 0;
        virtual void error(std::string_view message) noexcept = 0;
        virtual void critical(std::string_view message) noexcept = 0;
    };
}

#endif // FINANCETECHNOLOGYPROJECTS_LOGGER_HPP