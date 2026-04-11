/**============================================================================
Name        : Logger.cpp
Created on  : 21.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Logger.cpp
============================================================================**/

#include "Logger.h"
#include "Utils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <format>
#include <ctime>
#include <iomanip>

#ifndef CROW_LOG_LEVEL
#define CROW_LOG_LEVEL 1
#endif

#define CROW_ENABLE_LOGGING

namespace Logger::crow
{
    enum class LogLevel
    {
#ifndef ERROR
#ifndef DEBUG
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
#endif
#endif

        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler
    {
    public:
        virtual ~ILogHandler() = default;

        virtual void log(std::string message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler
    {
    public:
        void log(std::string message, LogLevel level) override
        {
            std::string prefix;
            switch (level)
            {
                case LogLevel::Debug:
                    prefix = "DEBUG   ";
                    break;
                case LogLevel::Info:
                    prefix = "INFO    ";
                    break;
                case LogLevel::Warning:
                    prefix = "WARNING ";
                    break;
                case LogLevel::Error:
                    prefix = "ERROR   ";
                    break;
                case LogLevel::Critical:
                    prefix = "CRITICAL";
                    break;
            }
            std::cerr << std::string("(") + timestamp() + std::string(") [") + prefix + std::string("] ") + message << std::endl;
        }

    private:
        static std::string timestamp()
        {
            char date[32];
            time_t t = time(0);

            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
            #ifdef CROW_USE_LOCALTIMEZONE
            localtime_s(&my_tm, &t);
#else
            gmtime_s(&my_tm, &t);
#endif
#else
#ifdef CROW_USE_LOCALTIMEZONE
            localtime_r(&t, &my_tm);
#else
            gmtime_r(&t, &my_tm);
#endif
#endif

            size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
            return std::string(date, date + sz);
        }
    };

    class logger
    {
    public:
        logger(LogLevel level):
                level_(level)
        {}
        ~logger()
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                get_handler_ref()->log(stringstream_.str(), level_);
            }
#endif
        }

        //
        template<typename T>
        logger& operator<<(T const& value)
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                stringstream_ << value;
            }
#endif
            return *this;
        }

        //
        static void setLogLevel(LogLevel level) { get_log_level_ref() = level; }

        static void setHandler(ILogHandler* handler) { get_handler_ref() = handler; }

        static LogLevel get_current_log_level() { return get_log_level_ref(); }

    private:
        //
        static LogLevel& get_log_level_ref()
        {
            static LogLevel current_level = static_cast<LogLevel>(CROW_LOG_LEVEL);
            return current_level;
        }
        static ILogHandler*& get_handler_ref()
        {
            static CerrLogHandler default_handler;
            static ILogHandler* current_handler = &default_handler;
            return current_handler;
        }

        //
        std::ostringstream stringstream_;
        LogLevel level_;
    };
};

#define CROW_LOG_CRITICAL                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Critical) \
    crow::logger(crow::LogLevel::Critical)
#define CROW_LOG_ERROR                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Error) \
    crow::logger(crow::LogLevel::Error)
#define CROW_LOG_WARNING                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Warning) \
    crow::logger(crow::LogLevel::Warning)
#define CROW_LOG_INFO                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Info) \
    crow::logger(crow::LogLevel::Info)
#define CROW_LOG_DEBUG                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Debug) \
    crow::logger(crow::LogLevel::Debug)


namespace Logger::LoggerUtils
{
    std::string timestampOld()
    {
        char date[32];
        const time_t t = time(0);
        tm my_tm;
        gmtime_r(&t, &my_tm);

        size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);

        return std::string(date, date + sz);
    }

    std::string timestampNew()
    {
        const time_t t = time(nullptr);
        tm my_tm;
        gmtime_r(&t, &my_tm);

        std::string timestamp(24, '\0');
        // const size_t sz = strftime(timestamp.data(), timestamp.size(), "%Y-%m-%d %H:%M:%S", &my_tm);
        const size_t sz = strftime(&timestamp[0], timestamp.size(), "%Y-%m-%d %H:%M:%S", &my_tm);

        timestamp.resize(sz);
        //timestamp.shrink_to_fit();

        return timestamp;
    }

    std::string timestampX()
    {
        const std::time_t ct { std::time(nullptr) };

        return std::string(ctime(&ct));
    }

    void timestampX2()
    {
        std::time_t t = std::time(nullptr);
        std::cout << std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S");
    }

    std::string timestampFormat()
    {
        const std::chrono::time_point now = std::chrono::system_clock::now();
        return std::format("{:%d-%m-%Y %H:%M:%OS}", now);
    }

    void TimestampTests()
    {
        std::cout << timestampOld() << std::endl;
        std::cout << timestampNew() << std::endl;
        std::cout << timestampX() << std::endl;
        timestampX2();
        timestampFormat();
    }

    void PerfTest()
    {
        {
            Utils::ScopedTimer timer {"timestampOld()"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                auto x = timestampOld();
            }
        }

        {
            Utils::ScopedTimer timer {"timestampNew()"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                auto x = timestampNew();
            }
        }

        {
            Utils::ScopedTimer timer {"gmtime()"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                const std::time_t t = std::time(nullptr);
                auto x=  std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S");
            }
        }

        {
            Utils::ScopedTimer timer {"timestampFormat()"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                auto x= timestampFormat();
            }
        }
    }

    std::string get_timestamp_1()
    {
        return "2024-04-21 09:42:02";
    }

    void log_1(std::string message, crow::LogLevel level)
    {
        std::string levelStr;
        switch (level)
        {
            case crow::LogLevel::Debug:
                levelStr = "DEBUG   ";
                break;
            case crow::LogLevel::Info:
                levelStr = "INFO    ";
                break;
            case crow::LogLevel::Warning:
                levelStr = "WARNING ";
                break;
            case crow::LogLevel::Error:
                levelStr = "ERROR   ";
                break;
            case crow::LogLevel::Critical:
                levelStr = "CRITICAL";
                break;
        }
        std::string text = std::string("(") + timestampOld() + std::string(") [")
                + levelStr + std::string("] ") + message;

        //std::cout << text << std::endl;
    }

    std::string get_timestamp_2()
    {
        return "(2024-04-21 09:42:02)";
    }

    void log_2(std::string&& message, crow::LogLevel level)
    {
        const time_t t = time(nullptr);
        tm my_tm;
        gmtime_r(&t, &my_tm);

        std::string text(128, ' ');
        const size_t sz = strftime(&text[0], text.size(), "(%Y-%m-%d %H:%M:%S) ", &my_tm);
        // const size_t sz = 32;
        text.resize(sz);

        switch (level)
        {
            case crow::LogLevel::Debug:
                text.append("[DEBUG   ] ");
                break;
            case crow::LogLevel::Info:
                text.append("[INFO    ] ");
                break;
            case crow::LogLevel::Warning:
                text.append("[WARNING ] ");
                break;
            case crow::LogLevel::Error:
                text.append("[ERROR   ] ");
                break;
            case crow::LogLevel::Critical:
                text.append("[CRITICAL] ");
                break;
        }
        text.append(message);

        //std::cout << text << std::endl;
    }

    void logTest()
    {
        log_1("SomeTestMessage", crow::LogLevel::Debug);
        log_2("SomeTestMessage", crow::LogLevel::Debug);
    }

    void LogPerfTest()
    {
        {
            Utils::ScopedTimer timer {"log_1)"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                log_1("SomeTestMessage", crow::LogLevel::Debug);
            }
        }

        {
            Utils::ScopedTimer timer {"log_1)"};
            for (int i = 0; i < 10'000'000; ++i)
            {
                log_2("SomeTestMessage", crow::LogLevel::Debug);
            }
        }
    }

}


void Logger::TestAll()
{
    // LoggerUtils::TimestampTests();
    // LoggerUtils::PerfTest();

    // LoggerUtils::logTest();
    LoggerUtils::LogPerfTest();




    /*
    CROW_LOG_CRITICAL << "12323";
    CROW_LOG_ERROR << "12323";
    CROW_LOG_WARNING << "12323";
    CROW_LOG_INFO << "12323";
    CROW_LOG_DEBUG << "12323";
    */
};