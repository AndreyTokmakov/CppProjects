/**============================================================================
Name        : Strings.h
Created on  : 20.11.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Strings texts and experiments
============================================================================**/

#include "Strings.h"
#include "StringUtilities.h"

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
    class Literal
    {
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


namespace StringUtilities
{
    void slice_string(std::string &str, size_t from, size_t until)
    {
        if (!(str.length() > until && until > from))
            return;

        size_t pos = 0;
        for (size_t idx = from; idx <= until; ++idx)
            str[pos++] = str[idx];
        str.resize(pos);
        str.shrink_to_fit();
    }
}


namespace Strings::UtilitiesTests
{
    using namespace StringUtilities;

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

    void trim_string_test()
    {
        for (const std::string& base: std::vector<std::string>{
                "   Some   Sample    String  "
        })
        {   std::cout << "Input: " << std::quoted(base) << std::endl;

            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_1(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_2(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_3(str);
                std::cout << std::quoted(str) << std::endl;
            }
        }
    }
    void strip_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        strip(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void remove_chars_from_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        remove_chars_from_string(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void Update_string_test()
    {
        std::string str { "0123456789___________________" };
        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;

        slice_string(str, 3, 8);

        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;
    }

    void Random_String()
    {
        for (int i = 10; i < 20; ++i)
            std::cout << randomString(i) << std::endl;
    }
}



namespace Strings::Size_and_Capacity_after_Move
{
    void sink(std::string&& str)
    {
        std::string tmp = std::move(str);
        tmp.clear();
    }

    void info(const std::string& str)
    {
        std::cout << "size: " << str.size() << ", capacity: " << str.capacity() << std::endl;
    }

    void Test()
    {

        std::string buffer { "11111111111111111111111111111111111"};

        sink(std::move(buffer));
        info(buffer);

        buffer.append("1111111111111111111111111111111111111111");
        buffer.append("1111111111111111111111111111111111111111");

        info(buffer);
        sink(std::move(buffer));
        info(buffer);
    }
}


namespace Parse_ConnectorType
{

    std::string_view extractType(const std::string_view message)
    {
        using namespace std::string_view_literals;
        size_t pos = message.find(R"("connector")"sv);
        if (std::string::npos == pos)
            return {};

        const size_t start = message.find('\"', pos + 11);
        if (std::string::npos == start)
            return {};

        pos = message.find('\"', start + 1);
        if (std::string::npos == pos)
            return {};

        return message.substr(start + 1, pos - start - 1);
    }


    void ParseMessage()
    {
        const std::string message { R"({"connector": "Binance", "type": 2, "data": []})" };
        std::cout << message << std::endl;
        std::cout << extractType(message) << std::endl;
    }
}




void Strings::TestAll()
{
    // GetStringLengthAsCompileTime();
    // std::cout << sizeof(std::string) << std::endl;

    // UtilitiesTests::split_test_1();
    // UtilitiesTests::strip_string_test();
    // UtilitiesTests::trim_string_test();
    // UtilitiesTests::remove_chars_from_string_test();
    // UtilitiesTests::Update_string_test();
    // UtilitiesTests::Random_String();

    Parse_ConnectorType::ParseMessage();
};
