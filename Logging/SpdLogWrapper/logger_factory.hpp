/**============================================================================
Name        : logger_factory.hpp
Created on  : 23.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Factory for application logging infrastructure.
============================================================================**/

#ifndef FINANCETECHNOLOGYPROJECTS_LOGGER_FACTORY_HPP
#define FINANCETECHNOLOGYPROJECTS_LOGGER_FACTORY_HPP

#include "logger.hpp"

#include <memory>
#include <string>

namespace spd_log_wrapper
{
    struct LoggerConfiguration
    {
        std::string name { "trading" };
        std::string fileName { "/home/andtokm/DiskS/Temp/Logs/trading.log" };

        std::size_t queueSize { 8192 };
        std::size_t workerThreads { 1 };

        std::size_t maxFileSize { 16 * 1024 * 1024 };
        std::size_t maxFiles { 3 };
    };

    [[nodiscard]]
    std::shared_ptr<ILogger> createLogger(const LoggerConfiguration& configuration = {});
}

#endif // FINANCETECHNOLOGYPROJECTS_LOGGER_FACTORY_HPP