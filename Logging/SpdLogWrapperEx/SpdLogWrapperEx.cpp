/**============================================================================
Name        : SpdLogWrapperEx.cpp
Created on  : 29.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpdLogWrapperEx.cpp
============================================================================**/

#include "SpdLogWrapperEx.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/version.h>


#include <iostream>
#include <memory>
#include <string>
#include <utility>


namespace logging
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        template<typename... Args>
        void trace(fmt::format_string<Args...> format, Args&&... args)
        {
            traceImpl(fmt::format(format, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void debug(fmt::format_string<Args...> format, Args&&... args)
        {
            debugImpl(fmt::format(format, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void info(fmt::format_string<Args...> format, Args&&... args)
        {
            infoImpl(fmt::format(format, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void warn(fmt::format_string<Args...> format, Args&&... args)
        {
            warnImpl(fmt::format(format, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void error(fmt::format_string<Args...> format, Args&&... args)
        {
            errorImpl(fmt::format(format, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void critical(fmt::format_string<Args...> format, Args&&... args)
        {
            criticalImpl(fmt::format(format, std::forward<Args>(args)...));
        }

    private:
        virtual void traceImpl(std::string message) = 0;
        virtual void debugImpl(std::string message) = 0;
        virtual void infoImpl(std::string message) = 0;
        virtual void warnImpl(std::string message) = 0;
        virtual void errorImpl(std::string message) = 0;
        virtual void criticalImpl(std::string message) = 0;
    };


    class SpdlogLogger final : public ILogger
    {
    public:
        explicit SpdlogLogger(std::shared_ptr<spdlog::logger> logger)
            : m_logger(std::move(logger))
        {
        }

    private:
        void traceImpl(const std::string message) override
        {
            m_logger->trace(message);
        }

        void debugImpl(const std::string message) override
        {
            m_logger->debug(message);
        }

        void infoImpl(const std::string message) override
        {
            m_logger->info(message);
        }

        void warnImpl(const std::string message) override
        {
            m_logger->warn(message);
        }

        void errorImpl(const std::string message) override
        {
            m_logger->error(message);
        }

        void criticalImpl(const std::string message) override
        {
            m_logger->critical(message);
        }

        std::shared_ptr<spdlog::logger> m_logger;
    };
}


void spd_log_wrapper_ex::TestAll()
{
    std::shared_ptr<logging::ILogger> logger = std::make_shared<logging::SpdlogLogger>(spdlog::default_logger());

    logger->trace("Trace message: {}", 1);
    logger->debug("Debug message: {}", 2);
    logger->info("Info message: {}", 3);
    logger->warn("Warning: order {} is close to limit", 12345);
    logger->error("Failed to process order {}, error code {}", 12345, -1);
    logger->critical("Critical error: {}", "exchange connection lost");
}
