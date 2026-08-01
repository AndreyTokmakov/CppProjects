/**============================================================================
Name        : PacketProcessing_Pipeline.cpp
Created on  : 01.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PacketProccesing_Pipeline
============================================================================**/

#include "ChainOfResponsibility.hpp"

#include <iostream>
#include <memory>
#include <tuple>

namespace
{
    constexpr std::size_t MaxPayloadSize = 1024;
    constexpr uint32_t ExpectedProtocolVersion = 1;
    constexpr uint32_t MaxRequestsPerSecond = 100;

    struct Packet
    {
        uint32_t protocolVersion;
        uint32_t clientId;
        std::string token;
        std::string payload;
    };

    struct ProcessingContext
    {
        uint32_t requestsProcessed { 0 };
        bool authenticated { false };
    };

    enum class HandlerResult: uint8_t{
        Continue,
        Stop
    };

    enum class PipelineResult: uint8_t
    {
        Completed,
        Stopped
    };

    struct HeaderValidator
    {
        [[nodiscard]]
        HandlerResult process(const Packet& packet, ProcessingContext&) const noexcept
        {
            if (packet.protocolVersion != ExpectedProtocolVersion) {
                std::cout << "Invalid protocol version\n";
                return HandlerResult::Stop;
            }

            if (packet.payload.size() > MaxPayloadSize) {
                std::cout << "Payload is too large\n";
                return HandlerResult::Stop;
            }

            std::cout << "Header is valid\n";
            return HandlerResult::Continue;
        }
    };

    struct AuthenticationHandler
    {
        [[nodiscard]]
        HandlerResult process(const Packet& packet, ProcessingContext& context) const noexcept
        {
            if (packet.token.empty())
            {
                std::cout << "Authentication failed\n";
                return HandlerResult::Stop;
            }

            context.authenticated = true;

            std::cout << "Client authenticated\n";
            return HandlerResult::Continue;
        }
    };

    struct RateLimitHandler
    {
        [[nodiscard]]
        HandlerResult process(Packet&, ProcessingContext& context) const noexcept
        {
            if (context.requestsProcessed >= MaxRequestsPerSecond)
            {
                std::cout << "Rate limit exceeded\n";
                return HandlerResult::Stop;
            }

            ++context.requestsProcessed;

            std::cout << "Rate limit check passed\n";
            return HandlerResult::Continue;
        }
    };

    struct CommandHandler
    {
        [[nodiscard]]
        HandlerResult process(const Packet& packet, const ProcessingContext& context) const noexcept
        {
            if (!context.authenticated)
            {
                std::cout << "Client is not authenticated\n";
                return HandlerResult::Stop;
            }

            std::cout << "Executing command: " << packet.payload << '\n';

            return HandlerResult::Continue;
        }
    };

    template<typename... Handlers>
    struct PacketPipeline
    {
        [[nodiscard]]
        PipelineResult process(Packet& packet, ProcessingContext& context) const noexcept
        {
            return processImpl<0>(packet, context);
        }

    private:

        template<std::size_t INDEX>
        [[nodiscard]]
        PipelineResult processImpl(Packet& packet, ProcessingContext& context) const noexcept
        {
            if constexpr (INDEX == sizeof...(Handlers))
            {
                return PipelineResult::Completed;
            }
            else
            {
                const auto& handler = std::get<INDEX>(handlers_);
                if (handler.process(packet, context) == HandlerResult::Stop) {
                    return PipelineResult::Stopped;
                }
                return processImpl<INDEX + 1>(packet, context);
            }
        }

        std::tuple<Handlers...> handlers_;
    };
}

void chain_of_responsibility::packet_processing_pipeline::testAll()
{
    constexpr PacketPipeline<HeaderValidator,
                             AuthenticationHandler,
                             RateLimitHandler,
                             CommandHandler> pipeline;

    auto processPacket = [&](Packet& packet)
    {
        ProcessingContext context;
        const PipelineResult result = pipeline.process(packet, context);
        if (result == PipelineResult::Completed) {
            std::cout << "Packet processing completed\n";
        } else {
            std::cerr << "Error: Packet processing stopped\n";
        }
    };


    Packet packet1 {
        .protocolVersion = 1,
        .clientId = 42,
        .token = "secret",
        .payload = "SET_FREQUENCY 915000000"
    };

    processPacket(packet1);

    std::cout << std::string(120, '=') << std::endl;

    Packet packe2t {
        .protocolVersion = 1,
        .clientId = 42,
        .token = "secret",
        .payload = std::string(2 * 1024, '0')
    };

    processPacket(packe2t);

    /**
    Header is valid
    Client authenticated
    Rate limit check passed
    Executing command: SET_FREQUENCY 915000000
    Packet processing completed
    ========================================================================================================================
    Payload is too large
    Error:  Packet processing stopped
    **/
}