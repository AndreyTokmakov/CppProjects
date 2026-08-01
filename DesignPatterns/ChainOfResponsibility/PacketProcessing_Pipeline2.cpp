/**============================================================================
Name        : PacketProcessing_Pipeline2.cpp
Created on  : 01.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PacketProcessing_Pipeline2
============================================================================**/

#include "ChainOfResponsibility.hpp"

#include <iostream>
#include <memory>
#include <tuple>
#include <expected>

namespace
{
    constexpr std::size_t MaxPacketSize = 1024;
    constexpr std::size_t MinPacketSize = 4;
    constexpr uint8_t ProtocolVersion = 1;
    constexpr uint8_t AuthenticatedFlag = 0x01;

    enum class Error: uint8_t
    {
        Success,
        PacketTooSmall,
        PacketTooLarge,
        InvalidCrc,
        InvalidVersion,
        NotAuthenticated,
        UnknownCommand
    };

    struct Packet
    {
        uint8_t version { 0 };
        uint8_t flags { 0 };
        uint16_t payloadSize { 0 };
        uint16_t crc { 0 };
        std::string_view payload {};
    };

    struct ProcessingContext
    {
        bool authenticated { false };
        uint16_t commandId { 0 };
    };

    using HandlerResult = std::expected<void, Error>;

    template<typename Handler>
    concept PacketHandlerConcept = requires(const Handler& handler, Packet& packet, ProcessingContext& context)
    {
        { handler.process(packet, context) } -> std::same_as<HandlerResult>;
    };

    struct LengthValidator
    {
        [[nodiscard]]
        constexpr
        HandlerResult process(const Packet& packet, ProcessingContext&) const noexcept
        {
            const auto packetSize = sizeof(Packet) + packet.payload.size();
            if (packetSize < MinPacketSize) {
                return std::unexpected(Error::PacketTooSmall);
            }

            if (packetSize > MaxPacketSize){
                return std::unexpected(Error::PacketTooLarge);
            }

            return {};
        }
    };

    struct CrcValidator
    {
        [[nodiscard]]
        constexpr HandlerResult process(const Packet& packet, ProcessingContext&) const noexcept
        {
            constexpr uint16_t ExpectedCrc = 0x1234;
            if (packet.crc != ExpectedCrc) {
                return std::unexpected(Error::InvalidCrc);
            }

            return {};
        }
    };

    struct HeaderValidator
    {
        [[nodiscard]]
        constexpr HandlerResult process(const Packet& packet, ProcessingContext&) const noexcept
        {
            if (packet.version != ProtocolVersion) {
                return std::unexpected(Error::InvalidVersion);
            }

            return {};
        }
    };

    struct AuthenticationValidator
    {
        [[nodiscard]]
        constexpr HandlerResult process(const Packet& packet, ProcessingContext& context) const noexcept
        {
            context.authenticated = (packet.flags & AuthenticatedFlag) != 0;
            if (!context.authenticated) {
                return std::unexpected(Error::NotAuthenticated);
            }

            return {};
        }
    };

    struct CommandDecoder
    {
        constexpr static std::size_t CommandSizeMin = 2;
        constexpr static std::size_t CommandSizeMax = 1024;

        [[nodiscard]]
        constexpr HandlerResult process(const Packet& packet, ProcessingContext& context) const noexcept
        {
            if (CommandSizeMin > packet.payload.size() || packet.payload.size() > CommandSizeMax) {
                std::cerr << packet.payload.size() << '\n';
                return std::unexpected(Error::UnknownCommand);
            }

            context.commandId =
                static_cast<uint16_t>(static_cast<uint8_t>(packet.payload[0])) |
                static_cast<uint16_t>(static_cast<uint8_t>(packet.payload[1])) << 8;

            return {};
        }
    };

    struct PacketHandler
    {
        [[nodiscard]]
        HandlerResult process(Packet&, const ProcessingContext& context) const noexcept
        {
            std::cout << "Executing command: " << context.commandId << '\n';
            return {};
        }
    };

    template<PacketHandlerConcept... Handlers>
    class PacketPipeline
    {
    public:

        [[nodiscard]]
        HandlerResult process(Packet& packet, ProcessingContext& context) const noexcept
        {
            return processImpl<0>(packet, context);
        }

    private:

        template<std::size_t INDEX>
        [[nodiscard]]
        HandlerResult processImpl(Packet& packet, ProcessingContext& context) const noexcept
        {
            if constexpr (INDEX == sizeof...(Handlers)) {
                return {};
            }
            else
            {
                const auto& handler = std::get<INDEX>(handlers_);
                if (const auto result = handler.process(packet, context); !result) {
                    return result;
                }

                return processImpl<INDEX + 1>(packet, context);
            }
        }

        std::tuple<Handlers...> handlers_;
    };

    [[nodiscard]]
    constexpr std::string_view errorToString(const Error error) noexcept
    {
        switch (error)
        {
            case Error::Success:
                return "Success";
            case Error::PacketTooSmall:
                return "PacketTooSmall";
            case Error::PacketTooLarge:
                return "PacketTooLarge";
            case Error::InvalidCrc:
                return "InvalidCrc";
            case Error::InvalidVersion:
                return "InvalidVersion";
            case Error::NotAuthenticated:
                return "NotAuthenticated";
            case Error::UnknownCommand:
                return "UnknownCommand";
        }

        return "Unknown";
    }
}

void chain_of_responsibility::packet_processing_pipeline_2::testAll()
{
    constexpr PacketPipeline<LengthValidator,
                             CrcValidator,
                             HeaderValidator,
                             AuthenticationValidator,
                             CommandDecoder,
                             PacketHandler> pipeline;

    Packet packet {
        .version = ProtocolVersion,
        .flags = AuthenticatedFlag,
        .payloadSize = 2,
        .crc = 0x1234,
        .payload = "43232323232323"
    };

    ProcessingContext context;
    const HandlerResult result = pipeline.process(packet, context);
    if (result) {
        std::cout << "Packet processing completed\n";
        return;
    }

    std::cout << "Packet processing failed: " << errorToString(result.error()) << '\n';
}