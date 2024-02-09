/**============================================================================
Name        : Strings.h
Created on  : 20.11.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Strings texts and experiments
============================================================================**/

#include "Strings.h"

#include <iostream>
#include <string_view>
#include <list>
#include <vector>
#include <stdint.h>
#include <iomanip>
#include <algorithm>
#include <array>


namespace
{
    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::vector<T> &list)
    {
        for (const auto &i: list)
            stream << i << ' ';
        return stream;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::list<T> &list)
    {
        for (const auto &i: list)
            stream << " " << i;
        return stream;
    }
}


namespace Strings::Utilities
{
    std::vector<std::string> split(std::string_view input,
                                   std::string_view delimiter = " ")
    {
        std::vector<std::string> output;
        for (size_t first = 0; first < input.size(); ) {
            const auto second = input.find_first_of(delimiter, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }

    // TODO: Check for performance: delimiter : std::string ---> std::string_view
    [[nodiscard]]
    std::vector<std::string> split(const std::string &str,
                                   const size_t partsExpected = 10,
                                   const std::string& delimiter = std::string {";"})
    {
        std::vector<std::string> parts{};
        parts.reserve(partsExpected);
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            parts.emplace_back(str, prev, pos - prev);
            prev = pos + delimiter.length();
        }
        parts.emplace_back(str, prev, str.length() - prev);
        return parts;
    }

    // TODO: Check for performance: delimiter : std::string ---> std::string_view
    void split_to(const std::string &str,
                  std::vector<std::string_view>& parts,
                  const std::string& delimiter = std::string {";"})
    {
        parts.clear();
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            parts.emplace_back(str.data() + prev, pos - prev);
            prev = pos + delimiter.length();
        }
        parts.emplace_back(str.data() + prev, str.length() - prev);
    }

    std::string_view strip(const std::string& str)
    {
        std::string::size_type start = 0, end = str.length() - 1;
        while (str.length() > start && str[start] == ' ') { ++start; }
        while (end && str[end] == ' ') { --end; }
        return std::string_view {str.data() + start, end - start + 1};
    }

    void strip(std::string& str)
    {
        std::string::size_type start = 0, length  = str.length();
        while (length > start && str[start] == ' ') { ++start; }
        str.erase(0, start);;

        std::string::size_type end = length - start - 1;
        while (end && str[end] == ' ') { --end; }
        str.erase(end + 1, length - start - end);

        str.shrink_to_fit();
    }

    // Remove ' ', '\t', '\n', '\r', '\n' symbols from the END and BEGINING of the string
    void strip_ex(std::string& str)
    {
        constexpr std::array<char, 5> symbols { ' ', '\t', '\n', '\r', '\n'};

        std::string::size_type start = 0, length  = str.length();
        while (length > start && std::any_of(symbols.cbegin(), symbols.cend(), [&](const char c) {
            return c == str[start]; })) { ++start; }
        str.erase(0, start);;

        std::string::size_type end = length - start - 1;
        while (end  && std::any_of(symbols.cbegin(), symbols.cend(), [&](const char c) {
            return c == str[end]; })) { --end; }
        str.erase(end + 1, length - start - end);

        str.shrink_to_fit();
    }

    constexpr std::array<char, 256> toExclude = []() -> std::array<char, 256> {
        std::array<char, 256> tmp {};
        for (const char c: {'\t', '\n', '\r', '\n'})
            tmp[c] = 1;
        return tmp;
    } ();

    void remove_chars_from_string(std::string& str)
    {
        size_t index = 0;
        for (char c: str) {
            if (0 == toExclude[c])
                str[index++] = c;
        }
        str.resize(index);
        str.shrink_to_fit();
    }
}

namespace Strings
{
    constexpr size_t getLength(const char*)
    {
        return 2;
    }

    constexpr std::size_t constexprLength1(std::string_view str)
    {
        return std::char_traits<char>::length(str.data());
        // return std::string::traits_type::length(s);
    }

    constexpr std::size_t constexprLength2(std::string_view str)
    {
        return std::string::traits_type::length(str.data());
    }

    void GetStringLengthAsCompileTime()
    {
        constexpr std::string_view text { "123456789123456789123456789"};

        // static_assert(text.size() == constexprLength(text));
    }

    constexpr bool checkSSO()
    {
        // constexpr std::string sd;
        return true;
    }
};

namespace Strings::Literals
{

    class Literal {
        const char* const mData{};
        const size_t      mLen{};

        friend constexpr Literal operator""_l(const char* data, size_t len);
        friend constexpr bool operator==(const Literal& l1, const Literal& l2);

        constexpr Literal(const char* data, size_t len)
                : mData { data }, mLen { len } {
        }

    public:
        // imagine the access functions you need
    };

    constexpr Literal operator""_l(const char* data, size_t len){
        return {data, len};
    }

    constexpr bool operator==(const Literal &l1, const Literal &l2) {
        // TODO: Add str content check
        return l1.mLen == l2.mLen;
    }

    void Insert([[maybe_unused]] Literal str,
                [[maybe_unused]] size_t len) {}

    void Test()
    {
        Insert("Hello"_l, 20);
        static_assert("Hello"_l == "Hello"_l);
    }
}

namespace Strings::UtilitiesTests
{
    using namespace Strings::Utilities;

    void split_test_1()
    {

        const std::string text { "11_22_33_44" };

        {
            const std::vector<std::string> parts = split(text, "_");
            std::cout << parts << std::endl;
        }

        {
            const std::vector<std::string> parts = split(text, 10,"_");
            std::cout << parts << std::endl;
        }

        {
            std::vector<std::string_view> parts;
            split_to(text, parts,"_");
            std::cout << parts << std::endl;
        }
    }

    void strip_string_test()
    {
        const std::string str1 { "  A good   examplE    " };
        std::cout << strip(str1) << std::endl;
    }

    void trim_string_test()
    {
        std::string str1 { "  A good   examplE    " };

        std::cout << std::quoted(str1) << std::endl;
        strip(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void strip_ex_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        strip_ex(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void remove_chars_from_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        remove_chars_from_string(str1);
        std::cout << std::quoted(str1) << std::endl;
    }
}

void Strings::TestAll()
{
    // GetStringLengthAsCompileTime();
    // std::cout << sizeof(std::string) << std::endl;

    // UtilitiesTests::split_test_1();
    // UtilitiesTests::strip_string_test();
    // UtilitiesTests::strip_ex_string_test();
    UtilitiesTests::remove_chars_from_string_test();
    // UtilitiesTests::trim_string_test();


    /*
    constexpr std::array<char, 5> symbols { ' ', '\t', '\n', '\r', '\n'};
    const bool result = std::any_of(symbols.cbegin(), symbols.cend(), [](const char c) { return c == ' '; });
    std::cout << std::boolalpha << result << std::endl;
    */
};
