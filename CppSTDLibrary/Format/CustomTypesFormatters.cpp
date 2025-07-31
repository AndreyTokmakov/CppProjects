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


namespace CustomTypesFormatters::Version_Flags
{
    struct Version
    {
        std::string strVersion { " 1.2.3" };
        int major { 0 };
        std::optional<int> minor { std::nullopt };
        std::optional<int> patch { std::nullopt };
    };
}

template <>
struct std::formatter<CustomTypesFormatters::Version_Flags::Version> {
    std::string _attributes;
    constexpr auto parse(std::format_parse_context& parse_context) {
        auto it = std::ranges::find(parse_context, '}');
        _attributes = std::string(parse_context.begin(), it);
        return it;
    }

    auto format(const CustomTypesFormatters::Version_Flags::Version& version,
                std::format_context& format_context) const
    {
        auto out = format_context.out();
        if (_attributes.empty()) {
            out = std::format_to(out, "{} {}", version.strVersion, version.major);
            return out;
        }

        for (auto n = 0u; n < _attributes.size(); ++n)
        {
            if (_attributes[n] == '%') {
                switch (_attributes[++n]) {
                    case 'l':
                        out = std::format_to(out, "{}", version.strVersion);
                        break;
                    case 'v':
                        out = std::format_to(out, "{}", version.major);
                        if (version.minor.has_value()) {
                            out = std::format_to(out, ".{}", version.minor.value());
                        }
                        if (version.patch.has_value()) {
                            out = std::format_to(out, ".{}", version.patch.value());
                        }
                        break;
                    case 'm':
                        out = std::format_to(out, "{}", version.major);
                        break;
                    case 'n':
                        out = std::format_to(out, "{}", version.minor.value());
                        break;
                    case 'p':
                        out = std::format_to(out, "{}", version.patch.value());
                        break;
                    case '%':
                        out = std::format_to(out, "%");
                        break;
                    default:
                        out = "ERROR";
                        break;
                }
            } else {
                out = std::format_to(out, "{}", _attributes[n]);
            }
        }
        return out;
    }
};

namespace CustomTypesFormatters::Version_Flags
{
    void format_and_print()
    {
        Version version {"11.22.33",  1, 2, 3 };


        std::cout << std::format("{}", version) << std::endl;
        std::cout << std::format("{:%l%m}", version) << std::endl;
        std::cout << std::format("{:%l%v}", version) << std::endl;
        std::cout << std::format("{:%l %v}", version) << std::endl;
        std::cout << std::format("{:%l %m.%n.%p}", version) << std::endl;

        // 11.22.33 1
        // 11.22.331
        // 11.22.331.2.3
        // 11.22.33 1.2.3
        // 11.22.33 1.2.3
    }
}


void CustomTypesFormatters::TestAll()
{
    // BitFieldStructFormatter::Test();
    // CustomTypes::FormatTypes();
    // DemoTwo::test();
    Version_Flags::format_and_print();
}