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
#include <vector>
#include <list>
#include <algorithm>

namespace
{
    template<typename T>
    std::ostream &operator<<(std::ostream &ostr, const std::vector<T> &list) {
        for (const auto &i: list)
            ostr << i << ' ';
        return ostr;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &ostr, const std::list<T> &list) {
        for (const auto &i: list)
            ostr << " " << i;
        return ostr;
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

namespace Strings::SplitTests
{
    std::vector<std::string> split(std::string_view input,
                                   std::string_view delims = " ")
    {
        std::vector<std::string> output;
        for (size_t first = 0; first < input.size(); ) {
            const auto second = input.find_first_of(delims, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }


    void split_test_1()
    {
        const std::string text { "11_22_33_44" };
        const std::vector<std::string> parts = split(text, "_");

        std::cout << parts << std::endl;
    }

}

void Strings::TestAll()
{
    // GetStringLengthAsCompileTime();
    // std::cout << sizeof(std::string) << std::endl;

    SplitTests::split_test_1();
};
