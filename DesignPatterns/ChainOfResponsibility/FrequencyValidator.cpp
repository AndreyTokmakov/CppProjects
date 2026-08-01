/**============================================================================
Name        : FrequencyValidator.cpp
Created on  : 01.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FrequencyValidator.cpp
============================================================================**/

#include "ChainOfResponsibility.hpp"

#include <iostream>
#include <memory>

namespace
{
    enum class Result: uint8_t {
        Continue,
        Stop
    };

    struct SetFrequencyCommand
    {
        uint64_t frequency;
    };

    struct Handler
    {
        virtual ~Handler() = default;

        Handler& setNext(Handler& handler) noexcept
        {
            next = &handler;
            return handler;
        }

        Result process(const SetFrequencyCommand& command) const
        {
            if (Result::Stop == handle(command)) {
                return Result::Stop;
            }
            if (next != nullptr){
                return next->process(command);
            }

            return Result::Continue;
        }

    protected:

        virtual Result handle(const SetFrequencyCommand& command) const = 0;

    private:

        Handler* next { nullptr };
    };

    class FrequencyValidator : public Handler
    {
    protected:

        Result handle(const SetFrequencyCommand& command) const override
        {
            static constexpr uint64_t MIN_FREQUENCY = 70'000'000;
            static constexpr uint64_t MAX_FREQUENCY = 6'000'000'000;

            if (command.frequency < MIN_FREQUENCY)
            {
                std::cout << "Frequency is too low\n";
                return Result::Stop;
            }

            if (command.frequency > MAX_FREQUENCY)
            {
                std::cout << "Frequency is too high\n";
                return Result::Stop;
            }

            std::cout << "Frequency is valid\n";
            return Result::Continue;
        }
    };

    class BusyValidator : public Handler
    {
    protected:

        Result handle(const SetFrequencyCommand&) const override
        {
            constexpr bool DEVICE_BUSY = false;

            if (DEVICE_BUSY)
            {
                std::cout << "Device is busy\n";
                return Result::Stop;
            }

            std::cout << "Device is ready\n";
            return Result::Continue;
        }
    };

    class ProgramPllHandler : public Handler
    {
    protected:

        Result handle(const SetFrequencyCommand& command) const override
        {
            std::cout << "PLL <- " << command.frequency << '\n';
            return Result::Continue;
        }
    };

    class LoggerHandler : public Handler
    {
    protected:

        Result handle(const SetFrequencyCommand& command) const override
        {
            std::cout << "Log: frequency = " << command.frequency << '\n';
            return Result::Continue;
        }
    };

}

void chain_of_responsibility::frequency_validator::testAll()
{
    FrequencyValidator frequencyValidator;
    BusyValidator busyValidator;
    ProgramPllHandler pllHandler;
    LoggerHandler loggerHandler;

    frequencyValidator.setNext(busyValidator)
        .setNext(pllHandler)
        .setNext(loggerHandler);

    frequencyValidator.process({
        .frequency = 915'000'000
    });

    /**
    Frequency is valid
    Device is ready
    PLL <- 915000000
    Log: frequency = 915000000
    **/
}