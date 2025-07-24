/**============================================================================
Name        : CustomTypesFormatters.cpp
Created on  : 03.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CustomTypesFormatters.cpp
============================================================================**/

#include "CustomTypesFormatters.h"

#include <format>
#include <iostream>
#include <string_view>

#define BIT_FIELD(name) bool name: 1


namespace BitFieldStructFormatter
{
    struct Permissions
    {
        BIT_FIELD(read) { false };
        BIT_FIELD(write) { false };
        BIT_FIELD(execute) { false };
        BIT_FIELD(modify) { false };
        BIT_FIELD(remove) { false };
        BIT_FIELD(rename) { false };
        BIT_FIELD(copy) { false };
        BIT_FIELD(share) { false };
    };
}

template<>
struct std::formatter<BitFieldStructFormatter::Permissions>
{
    static constexpr auto parse(const std::format_parse_context& ctx) {
        return ctx.begin();
    }

    static auto format(const BitFieldStructFormatter::Permissions& permissions,
                std::format_context& ctx)
    {
        return std::format_to(ctx.out(), "Permissions (\n\tRead: {}\n\tWrite: {}\n\tExecute: {}\n\tModify {}"
                                         "\n\tRemove: {}\n\tRename: {}\n\tCopy: {}\n\tShare: {}\n)",
                              permissions.read,
                              permissions.write,
                              permissions.execute,
                              permissions.modify,
                              permissions.remove,
                              permissions.rename,
                              permissions.copy,
                              permissions.share);
    }
};

namespace BitFieldStructFormatter
{

    void Test()
    {
        Permissions permissions{};

        std::cout << sizeof(Permissions) << std::endl;
        std::cout << std::format("{}", permissions) << std::endl;
    }
}


namespace CustomTypes
{
    struct Object { };
    struct Greeter { };

    struct Wrapper {
        int value;
    };
}



// Specialization for the formatter type:
template <>
struct std::formatter<CustomTypes::Object>
{   // We do not parse anything, but we still need to advance the iterator over the corresponding {} in the format.
    // This happens at compile-time.
    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        while (it != ctx.end() && *it != '}')
            ++it;
        return it;
    }

    // Runtime formatting, we simply add "CustomObject" to the buffer
    auto format(const CustomTypes::Object&, auto& ctx) const
    {
        return std::format_to(ctx.out(), "Object");
    }
};



// Since our wrapper is effectively an int  we can inherit from the int formatter.
template <typename CharT>
struct std::formatter<CustomTypes::Wrapper, CharT> : std::formatter<int, CharT>
{
    // parse() is inherited, we need a format method matching our type
    auto format(const CustomTypes::Wrapper& v, auto& ctx) const
    {
        return std::formatter<int, CharT>::format(v.value, ctx);
    }
};



// Custom format example: {}, {:u} or {:w}
template <> struct std::formatter<CustomTypes::Greeter>
{
    std::string_view object = "World";

    void unexpected_format_specification_for_greeter(){}

    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        while (it != ctx.end() && *it != '}') {
            switch (*it) {
                case 'w': object = "World"; break;
                case 'u': object = "Universe"; break;
                // we call a non-constexpr function to trigger an error
                default: unexpected_format_specification_for_greeter();
            }
            ++it;
        }
        return it;
    }

    auto format(const CustomTypes::Greeter&, auto& ctx) const
    {
        return std::format_to(ctx.out(), "Hello {}!", object);
    }
};


namespace CustomTypes
{

    void FormatTypes()
    {
        auto str1 = std::format("{}, {}", Object{}, Object{});
        std::cout << str1 << std::endl; //  "Object, Object"

        auto str2 = std::format("0x{:X}", Wrapper{255});
        std::cout << str2 << std::endl; //  str2 == "0xFF"

        auto str3 = std::format("{:w}", Greeter{});
        auto str4 = std::format("{:u}", Greeter{});
        std::cout << str3 << "  " << str4 << std::endl; // Hello World!  Hello Universe!

        // std::format("{:Q}", Greeter{}); // Wouldn't compile:
        // call to non-'constexpr' function
        // ...unexpected_format_specification_for_greeter()
    }
}


namespace CustomTypesFormatters::DemoTwo
{
    struct Parameters
    {
        int32_t version { 0 };
        std::string name;
        std::string description;
    };
}


template <>
struct std::formatter<CustomTypesFormatters::DemoTwo::Parameters>
{
    std::formatter<std::string> strFormatter;

    constexpr auto parse(std::format_parse_context& parse_context) {
        return strFormatter.parse(parse_context);
    }

    auto format(const CustomTypesFormatters::DemoTwo::Parameters& params,
                std::format_context& format_context) const
    {
        const std::string output = std::format("{} {} {}", params.version, params.name, params.description);
        return strFormatter.format(output, format_context);
    }
};

namespace CustomTypesFormatters::DemoTwo
{
    void test()
    {
        Parameters cpp {20, "Name-1", "Description-1"};
        std::cout << std::format("{}", cpp) << '\n';
    }
}


void CustomTypesFormatters::TestAll()
{
    // BitFieldStructFormatter::Test();
    // CustomTypes::FormatTypes();
    DemoTwo::test();
}