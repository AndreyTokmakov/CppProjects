/**============================================================================
Name        : ParsersVariantDemo.cpp
Created on  : 20.12.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ParsersVariantDemo.cpp
============================================================================**/

#include "Variant.hpp"

#include <iostream>
#include <variant>
#include <string>


namespace
{
    namespace common
    {
        struct Event
        {
            std::string data {};
        };

        enum class Exchange
        {
            Binance,
            ByBit,
            Deribit,
            GateIO,
            OKX,
        };
    }

    namespace parsers
    {
        using common::Event;

        template<typename T>
        concept ParserType = requires(T& parser, const std::string& data) {
            { parser.parse(data) } -> std::same_as<Event>;
        };

        struct BinanceParser
        {
            [[nodiscard]]
            Event parse(const std::string& data) const
            {
                std::cout << "BinanceParser::parse(" << data << ")" << std::endl;
                return { "BinaneData" };
            }
        };

        struct ByBitParser
        {
            [[nodiscard]]
            Event parse(const std::string& data) const
            {
                std::cout << "ByBitParser::parse(" << data << ")" << std::endl;
                return { "ByBitData" };
            }
        };
    }


    namespace processing
    {
        using common::Event;
        using common::Exchange;
        using parsers::ParserType;
        using parsers::BinanceParser;
        using parsers::ByBitParser;

        struct DataProcessor
        {
            using Packet = std::span<const std::byte>;
            using Data   = const std::string&;

            using Parser = std::variant<BinanceParser,ByBitParser>;

            explicit DataProcessor(Exchange exchange);

            void process(const std::string& data);
            // void process(const Packet packet);

        private:

            Parser parser;
        };

        DataProcessor::DataProcessor(const Exchange exchange)
        {
            switch (exchange) {
                case Exchange::Binance:
                    parser.emplace<BinanceParser>();
                    break;
                case Exchange::ByBit:
                    parser.emplace<ByBitParser>();
                    break;
                case Exchange::Deribit:
                    break;
                case Exchange::GateIO:
                    break;
                case Exchange::OKX:
                    break;
            }
        }

        void DataProcessor::process(const std::string& data)
        {
            const Event event = std::visit([&](const auto& parser) {
                    return parser.parse(data);
            },parser);

            std::cout << event.data << std::endl;
        }
    }
}

void Variant::ParsersVariantDemo::TestAll()
{

    const std::string buffer { "Some_Test_Buffer" };

    processing::DataProcessor handler1 { common::Exchange::Binance };
    handler1.process(buffer);

    processing::DataProcessor handler2 { common::Exchange::ByBit };
    handler2.process(buffer);

}
