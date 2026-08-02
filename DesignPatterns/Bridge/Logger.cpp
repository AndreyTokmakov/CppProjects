/**============================================================================
Name        : Logger.cpp
Created on  : 02.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :  src
============================================================================**/

#include <iostream>
#include "Bridge.hpp"

/*
 * Bridge Pattern — Logging Example
 *
 * Purpose:
 * --------
 * The Bridge pattern is a structural design pattern that separates an
 * abstraction from its implementation, allowing both parts to evolve
 * independently.
 *
 * This example demonstrates the pattern in the context of firmware logging.
 * The logging system has two independent dimensions:
 *
 *   1. What is being logged:
 *      - telemetry data;
 *      - device events;
 *      - other types of log information.
 *
 *   2. Where the information is sent:
 *      - UART;
 *      - UDP;
 *      - potentially TCP, a file, a ring buffer, shared memory, etc.
 *
 * Without Bridge, these two dimensions tend to become coupled. For example,
 * adding a new logging category and a new transport can lead to a growing
 * number of combinations such as TelemetryUartLogger, TelemetryUdpLogger,
 * EventUartLogger, EventUdpLogger, and so on.
 *
 * Architecture:
 * -------------
 *
 *                         Abstraction
 *                              │
 *                     ┌────────┴────────┐
 *                     │                 │
 *              TelemetryLogger      EventLogger
 *                     │                 │
 *                     └────────┬────────┘
 *                              │
 *                              ▼
 *                        ILoggerSink
 *                         /         \
 *                        /           \
 *                       ▼             ▼
 *                   UartSink       UdpSink
 *
 * Logger is the abstraction that owns a reference to ILoggerSink.
 * TelemetryLogger and EventLogger extend the abstraction with operations
 * specific to different types of logging data.
 *
 * ILoggerSink represents the implementation side of the Bridge.
 * UartSink and UdpSink provide concrete transport implementations.
 *
 * The two hierarchies are connected through composition rather than
 * inheritance between every possible combination:
 *
 *     TelemetryLogger ──────► UartSink
 *     TelemetryLogger ──────► UdpSink
 *     EventLogger     ──────► UartSink
 *     EventLogger     ──────► UdpSink
 *
 * This means that adding a new logging abstraction does not require new
 * transport-specific classes. Likewise, adding a new transport does not
 * require modifications to existing logging abstractions.
 *
 * Key Benefits:
 * -------------
 * - separates the abstraction from its implementation;
 * - prevents combinatorial class explosion;
 * - allows both hierarchies to evolve independently;
 * - makes transport implementations replaceable;
 * - makes the abstraction easier to test using a fake ILoggerSink;
 * - is well suited to hardware abstraction layers and firmware code.
 *
 * In this example, runtime polymorphism is used through ILoggerSink.
 * The abstraction stores a reference to the implementation and delegates
 * transport-specific work to it.
 *
 * The same architectural idea can also be implemented with templates when
 * the implementation is known at compile time. That approach can eliminate
 * virtual dispatch and may be preferable in latency-sensitive firmware.
 */

namespace
{
    enum class Error {
        Success,
        TransmissionError
    };

    enum class EventType {
        DeviceStarted,
        DeviceStopped,
        FrequencyChanged
    };

    struct Telemetry
    {
        uint64_t frequency;
        double temperature;
    };

    struct ILoggerSink
    {
        virtual ~ILoggerSink() = default;

        [[nodiscard]]
        virtual Error write(const std::string_view message) noexcept = 0;
    };

    struct UartSink final: ILoggerSink
    {
        [[nodiscard]]
        Error write(const std::string_view message) noexcept override
        {
            std::cout << "[UART] " << message << '\n';
            return Error::Success;
        }
    };

    struct UdpSink final: ILoggerSink
    {
        [[nodiscard]]
        Error write(const std::string_view message) noexcept override
        {
            std::cout << "[UDP ] " << message << '\n';
            return Error::Success;
        }
    };

    class Logger
    {
    public:

        explicit Logger(ILoggerSink& sink) noexcept : sink { sink } {
        }

        [[nodiscard]]
        Error log(const std::string_view message) const noexcept
        {
            return sink.write(message);
        }

    protected:

        ILoggerSink& sink;
    };

    struct TelemetryLogger final : public Logger
    {
        using Logger::Logger;

        [[nodiscard]]
        Error logTelemetry(const Telemetry& telemetry) const noexcept
        {
            std::cout << "Telemetry: frequency=" << telemetry.frequency << ", temperature=" << telemetry.temperature << '\n';
            return sink.write("Telemetry received");
        }
    };

    struct EventLogger final : public Logger
    {
        using Logger::Logger;

        [[nodiscard]]
        Error logEvent(const EventType event) const noexcept
        {
            switch (event)
            {
                case EventType::DeviceStarted:
                    return sink.write("Device started");
                case EventType::DeviceStopped:
                    return sink.write("Device stopped");
                case EventType::FrequencyChanged:
                    return sink.write("Frequency changed");
            }
            return Error::TransmissionError;
        }
    };
}

void bridge::logger::TestAll()
{
    UartSink uart;
    UdpSink udp;

    TelemetryLogger uartTelemetryLogger { uart };
    TelemetryLogger udpTelemetryLogger { udp };

    EventLogger uartEventLogger { uart };
    EventLogger udpEventLogger { udp };

    constexpr Telemetry telemetry{
        .frequency = 2'400'000'000,
        .temperature = 42.5
    };

    auto _ = uartTelemetryLogger.logTelemetry(telemetry);
    _ =udpTelemetryLogger.logTelemetry(telemetry);

    _ = uartEventLogger.logEvent(EventType::DeviceStarted);
    _ = udpEventLogger.logEvent(EventType::FrequencyChanged);
}