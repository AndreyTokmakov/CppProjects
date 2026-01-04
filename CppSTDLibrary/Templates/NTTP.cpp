/**============================================================================
Name        : NTTP.cpp
Created on  : 27.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : NTTP.cpp
============================================================================**/

// ReSharper disable CppMemberFunctionMayBeStatic
#include "Templates.hpp"

#include <iostream>
#include <array>
#include <numeric>


namespace Templates::NTTP
{
    template<int Val, typename T>
    constexpr T addValue(T x) {
        return x + Val;
    }

    void AddValue()
    {
        constexpr auto result = addValue<10>(5);
        std::cout << "5 + 10 = " << result << std::endl;
    }

    void TransformArray()
    {
        std::array<int, 10> data {1,2,3,4,5,6,7,8,9,10};
        std::transform(data.begin(), data.end(), data.begin(), addValue<10, int>);
        std::for_each(data.begin(), data.end(), [](int i) {std::cout << i << std::endl; });
    }
}


namespace Templates::NTTP::One
{
    template<std::array a> auto get_value() {
        return a[1];
    }

    // C++20: NTTP can be a literal class type
    template<std::array arr>
    constexpr auto sum() {
        return std::accumulate(arr.cbegin(), arr.cend(), 0);
    }

    void ArrayAsTemplateParam()
    {
        std::cout << get_value<std::array{2,3,4,5,6,7,8,9,10,11,12,13}>() << std::endl;

        // C++20: class template arguments are deduced at the call site
        static_assert(sum<std::array<double, 8>{3, 1, 4, 1, 5, 9, 2, 6}>() == 31.0);

        // C++20: NTTP argument deduction and CTAD
        static_assert(sum<std::array{2, 7, 1, 8, 2, 8}>() == 28);
    }
}


namespace NTTemplates::NTTPTP::Function_With_Param
{
    struct Config
    {
        int min;
        int max;
    };

    template<Config CFG>
    void Fun()
    {
        std::cout << "Min: " << CFG.min << ", max: " << CFG.max << std::endl;
    }

    void test()
    {
        Fun<{3, 9}>();
        Fun<Config{1, 2}>();
    }
}


namespace Templates::NTTP::Type_With_Param
{
    struct Config
    {
        int min;
        int max;
    };

    template<Config config>
    struct Task
    {
        void submit() {
            std::cout << config.min << " - " << config.max  << std::endl;
        }
    };

    void test()
    {
        Task<{3, 9}>{}.submit();
    }
}

namespace Templates::NTTP::Lambda_as_NTTP
{
    template<auto Func>
    struct PersonalBudget {
        double compute(std::uint32_t amt) {
            return Func(amt);
        }
    };


    void test ()
    {
        auto savings = [](int amt) -> decltype(auto) {
            return static_cast<double>(0.75*amt);
        };

        PersonalBudget<savings> savingsBudget{};

        auto saveResult = savingsBudget.compute(2300);
        std::cout << "Estimated Savings: " << saveResult << std::endl;
    }
}




void Templates::NTTP::TestAll()
{

    AddValue();
    TransformArray();

    // One::ArrayAsTemplateParam();
    // Function_With_Param::test();
    // Type_With_Param::test();
    // Lambda_as_NTTP::test();
};