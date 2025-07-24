//============================================================================
// Name        : Format.cpp
// Created on  : 09.07.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Format 
//============================================================================

#include <iostream>
#include <string>
#include <chrono>
#include <iterator>

#include <format>
#include <print>

#include "Format.h"
#include "CustomTypesFormatters.h"


namespace Format
{
    void SimpleTest() {
        auto s = std::format("{} {}!", "Hello", "world", "something"); // OK, produces "Hello world!"
        std::cout << s << std::endl;

        s = std::format("{} {} {}", "Hello", "world", "something"); // OK, produces "Hello world!"
        std::cout << s << std::endl;
    }

    void Test_2() {

        using namespace std::literals::chrono_literals;

        const std::string s1 = std::format("The answer is {}.\n", 42);
        std::cout << s1 << std::endl;

        const std::string s2 = std::format("I'd rather be {1} than {0}.\n", "right", "happy");
        std::cout << s2 << std::endl;

        std::string buffer;

        std::format_to(std::back_inserter(buffer), "Hello, C++{}!\n", "20");
        std::cout << buffer;
        buffer.clear();

        // std::format_to(std::back_inserter(buffer), "Hello, {0}::{1}!{2}\N", "std", "format_to()", "\n", "extra param(s)...");
        // std::cout << buffer;

        buffer.clear();
    }


    void Format_Numbers()
    {
        std::string str = std::format("{0:{1}.{2}}", 4.2f, 4, 5);
        std::cout << str << std::endl;

        std::string str2 = std::format("{:{}.{}}", 4.2f, 4, 5);
        std::cout << str2 << std::endl;

        std::string str3 = std::format("AAAA_{}_BBBB_{}", 123, 456);
        std::cout << str2 << std::endl;
    }

    void Format_To()
    {
        std::string buffer;
        {
            std::format_to(std::back_inserter(buffer), "Hello, C++ {}!\n", "20");
        }
        std::cout << buffer;

        {
            buffer = std::format("Hello, C++ {}!\n", "23");
        }
        std::cout << buffer;
    }

    void Format_to_N()
    {
        {
            std::string buffer;
            std::format_to_n(std::back_inserter(buffer), 5, "Hello, C++{}!\n", "20");
            std::cout << buffer << '\n';
        }

        {
            std::string buffer;
            std::format_to_n(std::back_inserter(buffer), 7, "123456789", "qwerty");
            std::cout << buffer << '\n';
        }
    }

    void Format_to_N_2()
    {
        std::array<char, 32> static_buffer{};
        std::format_to_n(static_buffer.begin(), 32,
                         "Today is {}, Expected temperature is {} Celsius",
                         "Tuesday", 51);

        std::string_view str2(static_buffer.begin(), static_buffer.end());
        std::cout << str2 << "\n"; // Today is Tuesday, Expected tempe
    }

    void Format_To_2()
     {
        std::string buffer;
        std::format_to(std::back_inserter(buffer), //< OutputIt
                    "Hello, {0}::{1}!{2}",      //< fmt
                        "std",                      //< arg {0}
                        "format_to()",              //< arg {1}
                        "\n",                       //< arg {2}
                        "extra param(s)...");       //< unused
        std::cout << buffer;
     }

    void Format_To_Vector_Str()
    {
        std::vector<char> buffer;
        int x = 42;
        std::format_to(std::back_inserter(buffer), "x ---> {}", x);

        std::string_view str1(buffer.begin(), buffer.end());
        std::cout << str1 << "\n"; // str1 == "x ---> 42"
    }

    void Format_To_STD_Cout()
    {
        std::format_to(std::ostream_iterator<char>(std::cout), "x ---> {}\n", 123); // x ---> 123
    }

    void VFormatTest1()
    {
        std::string formatString = "Hello, C++{}!\n";
        std::cout << std::vformat(formatString, std::make_format_args("23")) << '\n';
    }

    template<typename... Args>
    inline void println(const std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
    }

    void VFormatPrint()
    {
        println("{}{} {}{}{}", "Hello", ',', "C++", -1 + 2 * 3 * 4, '!');
    }

    void Make_Path()
    {
        constexpr std::string_view format { R"(C:\Projects\cpp\VTK_Tests\DATA_AND_EXAMPLES\Out\{0}.stl)"};
        {
            std::string path;
            std::format_to(std::back_inserter(path), format, 100500);
            std::cout << path << std::endl;
        }

        {
            const std::string path = std::format(format, 100500);
            std::cout << path << std::endl;
        }
    }


    void Experiments()
    {
        constexpr std::string_view format { R"(C:\Projects\cpp\VTK_Tests\DATA_AND_EXAMPLES\Out\{0}.stl)"};
        std::string path;

        std::format_to(std::back_inserter(path), format.data(), 1);
        std::cout << path << std::endl;
    }


    void Formatted_Size()
    {
        using namespace std::literals::string_view_literals;

        constexpr std::string_view formatter { "Value 1: {}, Value 2: {}, Value 3: {}"sv };

        const auto min_buffer_size_1 = std::formatted_size(formatter, 1, 2, 3);
        std::cout << min_buffer_size_1 << std::endl;

        const auto min_buffer_size_2 = std::formatted_size(formatter, 1'000, 2'000, 3'000);
        std::cout << min_buffer_size_2 << std::endl;

        const auto min_buffer_size_3 = std::formatted_size(formatter, 1'000'000, 2'000'000, 3'000'000);
        std::cout << min_buffer_size_3 << std::endl;
    }

    void Formatted_Size_2()
    {
        using namespace std::numbers;

        // calculate the required size to store the formatted text
        size_t sz = std::formatted_size("pi == {}", pi_v<double>);

        // allocate a big enough buffer (+ 1 for '\0')
        std::unique_ptr<char[]> buffer { std::make_unique_for_overwrite<char[]>(sz+1) };

        // format text into the buffer
        std::format_to(buffer.get(), "pi == {}", pi_v<double>);
        buffer[sz] = '\0'; // terminate the string

        // buffer.get() == "pi == 3.141592653589793"

        std::cout << buffer.get() << "\n";
    }

    void Formatted_Size_3()
    {
        using namespace std::numbers;

        const size_t sz = std::formatted_size("pi == {}", pi_v<double>);
        std::vector<char> buffer (sz + 1, '\0');
        std::format_to(buffer.data(), "pi == {}", pi_v<double>);
        std::cout << buffer.data() << "\n";
    }
};


namespace Format::Doubles
{
    void Format_Doubles_Precision()
    {
        // Typical formatting options are present (e.g. precision)
        auto pi5 = std::format("{:.5}", std::numbers::pi);   // pi5 == "3.1416"

        std::cout << "pi5 == " << std::quoted(pi5) << "\n";


        // [width, precision] Formatting arguments can be provided as part of the argument list
        for (const std::pair<int, int>& formatParams: std::vector<std::pair<int, int>>{{
            {10, 3}, {11, 4}, {12, 5}
        }})
        {
            auto pivar = std::format("{: ^{}.{}}", std::numbers::pi, formatParams.first, formatParams.second);
            std::cout << "pivar == " << std::quoted(pivar) << "\n";
        }
    }
}

namespace Format::Date_and_Time
{
    using namespace std::literals;

    void FormatTime()
    {
        std::chrono::hh_mm_ss c {16h + 32min + 10s};

        const std::string& str1 = std::format("{:%R}", c);
        const std::string& str2 = std::format("{:%T}", c);

        std::cout << str1 << "  " << str2 << std::endl;
    }

    void Format_TimePoint()
    {
        const std::chrono::time_point now = std::chrono::system_clock::now();
        std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << '\n';
    }
}

namespace CustomFormatters
{
    /*

    struct Index {
        unsigned int id_{ 0 };
    };

    template <>
    struct std::formatter<Index> : std::formatter<int> {
        auto format(const Index& id, std::format_context& ctx) const {
            return std::formatter<int>::format(id.id_, ctx);
        }
    };

    template <>
    struct std::formatter<Index> {
        // for debugging only
        formatter() { std::cout << "formatter<Index>()\n"; }

        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        auto format(const Index& id, std::format_context& ctx) const {
            return std::format_to(ctx.out(), "{}", id.id_);
        }
    };

    */
}

namespace Format::Fill_Formated_String
{

    void PrintString_WithSpaces()
    {
        for (const std::string& lvl: {"INFO", "DEBUG", "WARNING", "ERROR", "CRITICAL"})
        {
            std::cout << std::format("[{:8s}]", lvl) << std::endl;
        }
    }
}


namespace Format
{

    void Print_Formated_Pointers()
    {
        std::unique_ptr<int> intPtr { std::make_unique<int>(123) };
        int* ptr = intPtr.get();

        std::cout << &ptr << '\n';

        // std::cout << std::format("{:#018x}", ptr) << '\n'; // error in C++23

        std::cout << std::format("{:#018x}", reinterpret_cast<uintptr_t>(ptr)) << '\n';
        std::cout << std::format("{:#018X}", reinterpret_cast<uintptr_t>(ptr)) << '\n';
    }
}


namespace Format::Runtime_Format_Checks
{
    void test()
    {
        const std::string FORMAT { "val = {}" };
        const std::string result = std::format(std::runtime_format(FORMAT), 42);

        std::cout << result << std::endl;
    }

}


void Format::TestAll()
{
    CustomTypesFormatters::TestAll();

    // Format_Numbers();

    // Format_To();
    // Format_To_2();
    // Format_To_Vector_Str();
    // Format_To_STD_Cout();

    // Format_to_N();
    // Format_to_N_2();

    // Runtime_Format_Checks::test();

    // Format::VFormatTest1();
    // Format::VFormatPrint();

    // Doubles::Format_Doubles_Precision();

    // SimpleTest();
    // Test_2();

    // Make_Path();

    // Formatted_Size();
    // Formatted_Size_2();
    // Formatted_Size_3();

    // Experiments();

    // Date_and_Time::FormatTime();
    // Date_and_Time::Format_TimePoint();

    // Fill_Formated_String::PrintString_WithSpaces();

    // Print_Formated_Pointers();
}
