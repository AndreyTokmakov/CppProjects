/**============================================================================
Name        : VirtualCall_vs_Variant.cpp
Created on  : 20.12.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : VirtualCall_vs_Variant.cpp
============================================================================**/

#include "VirtualCall_vs_Variant.hpp"
#include "PerfUtilities.hpp"
#include <iostream>
#include <variant>

namespace
{
    constexpr uint64_t testIterCount = 1000'000'000;

    namespace virtual_call
    {
        struct IParser
        {
            virtual ~IParser() = default;
            virtual int parse(const std::string&) = 0;
        };

        struct ParserOne: public IParser
        {
            virtual int parse(const std::string&)
            {
                return 1;
            }
        };

        struct ParserTwo: public IParser
        {
            virtual int parse(const std::string&)
            {
                return 2;
            }
        };

        __attribute__((optimize("O1")))
        void test()
        {
            const std::string data("Data");

            {
                utilities::perf::ScopedTimer clock { "virtual_call" };

                std::unique_ptr<IParser> parser = std::make_unique<ParserOne>();
                for (uint32_t i = 0; i < testIterCount; ++i) {
                    const auto _ = parser->parse(data);
                }

                parser = std::make_unique<ParserTwo>();
                for (uint32_t i = 0; i < testIterCount; ++i) {
                    const auto _ = parser->parse(data);
                }
            }
        }
    }

    namespace variant_call
    {
        struct ParserOne {
            virtual int parse(const std::string&) const {
                return 1;
            }
        };

        struct ParserTwo {
            virtual int parse(const std::string&) const {
                return 2;
            }
        };

        __attribute__((optimize("O1")))
        void test()
        {
            const std::string data("virtual_call");
            std::variant<ParserOne, ParserTwo> varParser = ParserOne{};

            const auto parseEvent = [&](const auto& parser) {
                const auto _ = parser.parse(data);
            };

            {
                utilities::perf::ScopedTimer clock { "variant_call" };

                for (uint32_t i = 0; i < testIterCount; ++i) {
                    std::visit(parseEvent, varParser);
                }
                varParser = ParserTwo{};

                for (uint32_t i = 0; i < testIterCount; ++i) {
                    std::visit(parseEvent, varParser);
                }
            }
        }
    }
}

void VirtualCall_vs_Variant::benchmark()
{
    virtual_call::test();
    variant_call::test();

    //  With __attribute__((optimize("O0")))
    //      virtual_call  :  4.74756 seconds.
    //      variant_call  :  5.26013 seconds.

    //  With __attribute__((optimize("O1")))
    //      virtual_call  :  2.34782 seconds.
    //      variant_call  :  0.469445 seconds.
}
