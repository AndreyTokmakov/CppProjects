/*============================================================================
Name        : DebugLogger.h
Created on  : 11.07.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : DebugLogger
============================================================================**/

#include "DebugLogger.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>

namespace DebugLogger
{
    std::string_view getCurrentTime() noexcept {
        const time_t now { time(nullptr) };
        return {asctime(gmtime(&now)) };
    }

    void writeToFile(std::string_view logFile) {
        if (std::fstream out(logFile.data(), std::ios::out | std::ios::app); out.is_open() && out.good()) {
            out << getCurrentTime() << "   ___1";
        }
    }


    class LoggerOne {
    public:
        std::fstream out {};

        static constexpr std::string_view logFile { R"(/home/andtokm/tmp/trace.log)" };
        static constexpr std::string_view TIME_FORMAT { "%Y-%m-%d %H:%M:%S"};

        [[nodiscard]]
        static std::string getCurrentTimeAsStr() noexcept {
            const std::time_t& in_time_t { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
            std::ostringstream oss;
            oss << std::put_time(std::localtime(&in_time_t), LoggerOne::TIME_FORMAT.data());
            return oss.str();
        }

        LoggerOne() {
            out.open(logFile.data(), std::ios::out | std::ios::app);
            if (!(out.is_open() && out.good())) {
                throw std::runtime_error("Hmmmmmm");
            }
        }

        /** Foolproof: Disable delete. **/
        void operator delete(void *ptr) = delete;

    public:
        static LoggerOne* getLogger() noexcept {
            static LoggerOne instance {};
            return &instance;
        }

        template<typename T>
        auto add_space(std::fstream& stream, const T& arg) -> decltype(auto) {
            stream << ' ';
            return arg;
        };

        template<typename ...Args>
        void putLog(Args&&... args) {
            out << "[" << getCurrentTimeAsStr() << "]";
            (out << ... << add_space(out, std::forward<Args>(args))) << std::endl;
        }
    };


    class LoggerTwo {
    public:
        std::fstream out {};

        static constexpr std::string_view logFile { R"(/home/andtokm/tmp/trace.log)" };
        // static constexpr std::string_view TIME_FORMAT { "%Y-%m-%d %H:%M:%S"};

        [[nodiscard]]
        static std::ostringstream getStreamWithDate() noexcept {
            std::ostringstream oss;
            const std::time_t& in_time_t { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
            oss << "[" << std::put_time(std::localtime(&in_time_t), LoggerOne::TIME_FORMAT.data()) << "]";
            return oss;
        }

        template<typename T>
        auto add_space(std::ostringstream& stream, const T& arg) const -> decltype(auto) {
            stream << ' ';
            return arg;
        };

        // TODO: Test for performance (is it ok to return std::string?)
        template<typename ...Args>
        std::string getParamsAsString(Args&&... args) const noexcept {
            std::ostringstream oss { getStreamWithDate() };
            (oss << ... << add_space(oss, std::forward<Args>(args))) << std::endl;
            return oss.str();
        }

        LoggerTwo() {
            out.open(logFile.data(), std::ios::out | std::ios::app);
            if (!(out.is_open() && out.good())) {
                throw std::runtime_error("Hmmmmmm");
            }
        }

        /** Foolproof: Disable delete. **/
        void operator delete(void *ptr) = delete;

    public:
        static LoggerTwo* getLogger() noexcept {
            static LoggerTwo instance {};
            return &instance;
        }


        template<typename ...Args>
        void putLog(Args&&... args) {
            out << getParamsAsString(std::forward<Args>(args)...) << std::endl;
        }
    };
}


namespace DebugLogger::Tests {

    void TestOne()
    {
        auto logger  = LoggerOne::getLogger();
        logger->putLog(1);
        logger->putLog(1, 2);
        logger->putLog(1, 2, 3);
    }

    void TestTwo()
    {
        LoggerTwo* logger  = LoggerTwo::getLogger();
        logger->putLog(1);

    }
}

void DebugLogger::TestAll() {
    // writeToFile();

    // Tests::TestOne();

    Tests::TestTwo();

}
