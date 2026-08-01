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
#include <tuple>

namespace
{
    enum class HandlerResult: uint8_t {
        Continue,
        Stop
    };

    enum class PipelineResult: uint8_t {
        Completed,
        Stopped
    };

    struct SetFrequencyCommand
    {
        uint64_t frequency;
    };

    struct FrequencyValidator
    {
        constexpr static uint64_t MinFrequency = 70'000'000;
        constexpr static uint64_t MaxFrequency = 6'000'000'000;

        [[nodiscard]]
        constexpr HandlerResult process(const SetFrequencyCommand& command) const noexcept
        {
            if (command.frequency < MinFrequency || command.frequency > MaxFrequency){
                std::cout << "Frequency is out of range\n";
                return HandlerResult::Stop;
            }

            std::cout << "Frequency is valid\n";
            return HandlerResult::Continue;
        }
    };

    struct BusyValidator
    {
        [[nodiscard]]
        constexpr HandlerResult process(const SetFrequencyCommand&) const noexcept
        {
            constexpr bool DEVICE_BUSY = false;
            if (DEVICE_BUSY) {
                std::cout << "Device is busy\n";
                return HandlerResult::Stop;
            }

            std::cout << "Device is ready\n";
            return HandlerResult::Continue;
        }
    };

    struct ProgramPllHandler
    {
        [[nodiscard]]
        HandlerResult process(const SetFrequencyCommand& command) const noexcept {
            std::cout << "PLL <- " << command.frequency << '\n';
            return HandlerResult::Continue;
        }
    };

    struct LoggerHandler
    {
        [[nodiscard]]
        HandlerResult process(const SetFrequencyCommand& command) const noexcept {
            std::cout << "Log: frequency = " << command.frequency << '\n';
            return HandlerResult::Continue;
        }
    };

    template<typename... Handlers>
    struct Pipeline
    {
        constexpr Pipeline() noexcept = default;

        explicit constexpr Pipeline(Handlers... handlers) noexcept :
            handlers { std::move(handlers)... } {
        }

        [[nodiscard]]
        PipelineResult process(const SetFrequencyCommand& command) noexcept {
            return processImpl<0>(command);
        }

    private:

        template<std::size_t INDEX>
        [[nodiscard]]
        PipelineResult processImpl(const SetFrequencyCommand& command) noexcept
        {
            if constexpr (INDEX == sizeof...(Handlers))
            {
                return PipelineResult::Completed;
            }
            else
            {
                if (auto& handler = std::get<INDEX>(handlers); handler.process(command) == HandlerResult::Stop) {
                    return PipelineResult::Stopped;;
                }
                return processImpl<INDEX + 1>(command);
            }
        }

        std::tuple<Handlers...> handlers;
    };
}

void chain_of_responsibility::frequency_validator_pipeline::testAll()
{
    Pipeline<FrequencyValidator,
             BusyValidator,
             ProgramPllHandler,
             LoggerHandler> pipeline;

    const auto _ = pipeline.process(SetFrequencyCommand{
        .frequency = 915'000'000
    });

    /**
    Frequency is valid
    Device is ready
    PLL <- 915000000
    Log: frequency = 915000000
    **/
}