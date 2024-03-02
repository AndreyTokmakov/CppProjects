/**============================================================================
Name        : Cpp23_Features.cpp
Created on  : 26.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cpp23_Features
============================================================================**/

#include "Cpp23_Features.h"

#include <iostream>
#include <string_view>
#include <array>
#include <vector>
#include <coroutine>
#include <expected>
#include <ranges>
#include <utility>
#include <format>


// C++ 23:
// #include <expected>

#include <stacktrace>
// #include <flat_map>
// #include <flat_set>


namespace Cpp23_Features
{
    template <class T>
    int foo() {
        if constexpr (std::is_same_v<T, int>) {
            return 42;
        } else if constexpr (std::is_same_v<T, float>) {
            return 24;
        } else {
            static_assert(false, "T should be an int or a float");
        }
    }

    void Test()
    {
        foo<int>();
        // foo<std::string>();
    }

    struct S {
        static constexpr bool operator() (int x, int y) {
            return x < y;
        }
    };

    struct StaticArray
    {
        static inline constexpr std::array<int, 5> data { 1, 2, 3, 4 ,5 };

        static int operator[](const size_t index)
        {
            // static_assert(index > data.size());
            return data[index];
        }
    };

    void static_operator()
    {
        constexpr S s;
        static_assert (s (1, 2));

        constexpr bool result = S::operator()(1, 2);  // OK in C++23
        std::cout << std::boolalpha << result << std::endl;
    }


    void static_operator_lambda()
    {
        [[maybe_unused]]
        auto isEven = [](int i) static {return i % 2 == 0;};


        /** At the same time, you only declare a lambda static if it has no capture,
         *  if it has a capture then the compiler reminds you that you violated the rules: **/
        [[maybe_unused]]  const int ratio = 0;
        // auto isDivisableBy = [ratio](int i) static {return i % ratio == 0;};
    }


    void static_operator__square_brackets()
    {
        int x = StaticArray{}[1];
        std::cout << x << std::endl;

        // static_assert(2 == StaticArray{}[1]);
    }

    //-----------------------------------------------------------------------------//

    int&& g(int&& x)
    {
        return x;
    }

    void test2()
    {
        [[maybe_unused]]
        const char arr[] = u8"hi";
    }

    /*
    struct Array
    {
        static constexpr std::array<int, 10> data{
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10
        };

        int operator[](int idx, int = 5)
        {
            return data[idx];
        }
    };

    void Array_Operator_Default_Value()
    {
        Array array {};
        std::cout << array[5] << std::endl;
    }
    */

    struct A
    {
        int value { 123 };


        A& operator=(int v) {
            value = v;
            return *this;
        }

        explicit operator bool() {
            return true;
        }
    };

    void Wparentheses()
    {
        A a;
        if (a = 0) { // Warning Here

        }
    }


    struct T {};

    T fn()
    {
        [[maybe_unused]]
        T t;

        return std::move (T{});
    }
};

namespace Cpp23_Features::Multidimensional_Operator
{
    void simpleTest()
    {
        std::vector<std::vector<int>> numbers = {
                {1, 2, 3, 4, 5},
                {11, 12, 13, 14, 15}
        };

        // std::cout << numbers[0, 1] << '\n';

        std::cout << numbers[0][1] << '\n';
    }
}

namespace Cpp23_Features::StackTrace
{

    void test()
    {
        // auto x = std::stacktrace::current();

    }

}

namespace Cpp23_Features::Expected
{
    // Typical use case
    std::expected<std::string,std::error_code> fun(bool error = true) {
        if (error)
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        return {"Hello World!" };
    }

    void basics()
    {
        std::expected<int,int> v = 10;

        // Similar interface to std::optional
        // v.has_value() == true , v.value() == 10, *v == 10

        std::cout << std::boolalpha << "v.has_value() == " << v.has_value() << "\n";
        std::cout << "*v == " << *v << ", v.value() == " << v.value() << "\n";
    }


    void UnExpected()
    {
        std::expected<int,int> err = std::unexpected {10};
        // To distinguish the error, it has to be wrapped in std::unexpected

        std::cout << std::boolalpha << "\nerr.has_value() == " << err.has_value() << "\n"; // err.has_value() == false
        std::cout << "e.error() == " << err.error() << "\n";                               // err.error() == 10
    }

    void Non_Initialized()
    {
        std::expected<int,int> m;

        // std::expected always contains either a result or an error

        std::cout << "\nm.has_value() == " << m.has_value() << "\n";  // m.has_value() == true
        std::cout << "m.value() == " << m.value() << "\n";            // m.value() == int{} == 0
    }

    void No_DefaultValue()
    {
        struct NoDefault final {
            explicit constexpr NoDefault(int) {}
        };

        // Therefore if the result type cannot be default constructed
        // the resulting std::expected cannot be default constructed either.

        // std::expected<NoDefault,int> n; // Wouldn't compile
        std::expected<NoDefault,int> n{20}; // OK

        std::cout << "\nn.has_value() == " << n.has_value() << "\n";  // n.has_value() == true
    }
}


namespace CountingIterator
{
    void print(const std::string_view remark, const std::vector<std::string>& v)
    {
        const long size = std::ssize(v);
        std::cout << remark << '[' << size << "] { ";
        for (auto it = std::counted_iterator{std::cbegin(v), size}; it != std::default_sentinel; ++it)
            std::cout << *it << ", ";
        std::cout << "}\n";
    }

    void test_print()
    {
        const std::vector<std::string> values {"One", "Two", "Three", "Four", "Five"};
        print("src", values);
        std::vector<decltype(values)::value_type> dst;
        std::ranges::copy(std::counted_iterator{values.begin(), 3},
                          std::default_sentinel,
                          std::back_inserter(dst));
        print("dst", dst);
    }


    void test_simple()
    {
        const std::vector<std::string> values {"One", "Two", "Three", "Four", "Five"};
        for (auto iter = std::counted_iterator{values.begin(), 3}; std::default_sentinel != iter; ++iter)
        {
            std::cout << *iter << ' ';
        }
    }
}

namespace Cpp23_Features::Format
{
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
}

namespace Cpp23_Features::SizeT_Literals
{
    void Test()
    {

        std::vector<int> values {1,2,3};
        for (auto i = 0uz, count = values.size(); i < values.size(); ++i)
        {
            // i --> size_t
            static_assert(std::is_same_v<decltype(i), size_t>);
        }
    }
}


namespace ExplicitObjectParameters
{
    struct X
    {
        //template<typename Self>
        void foo(this X& self, int) {

        }
    };

}


void Cpp23_Features::TestAll()
{

    // Format::Format_to_N();
    // Format::VFormatTest1();
    // Format::VFormatPrint();

    // static_operator();
    // static_operator_lambda();

    // static_operator__square_brackets();

    // test2();

    // Array_Operator_Default_Value();

    // Wparentheses();

    // Multidimensional_Operator::simpleTest();

    /*
    Expected::basics();
    Expected::UnExpected();
    Expected::Non_Initialized();
    Expected::No_DefaultValue();
    */

    // CountingIterator::test_simple();
    // CountingIterator::test_print();

    // SizeT_Literals::Test();
};
