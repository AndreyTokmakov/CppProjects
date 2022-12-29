//============================================================================
// Name        : Templates.h
// Created on  : 02.02.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Templates
//============================================================================

#include "Templates.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <utility>

namespace Templates {

    template <typename R, typename ... Ts>
    constexpr auto matches(const R& range, Ts ... ts) -> decltype(auto) {
        return (std::count(std::begin(range), std::end(range), ts) + ...);
    }

    void MathingTests() {
        static_assert(3 == matches("abcdefg", 'a', 'd', 'f'));
#if 0
        static_assert(3 == matches("abcdefg", 'a', 'd', 'F'));
#endif
    }


    // Need partial specialization for this to work
    template <typename T>
    struct Logger;

    // Return type and argument list
    template <typename R, typename... Args>
    struct Logger<R(Args...)>
    {
        std::function<R(Args...)>    m_func;
        std::string                  m_name;

        Logger(std::function<R(Args...)> f, std::string n) :
                m_func { std::move(f) }, m_name { std::move(n) } {
        }

        R operator()(Args... args)
        {
            std::cout << "Entering " << m_name << std::endl;
            R result = m_func(args...);
            std::cout << "Exiting " << m_name << std::endl;
            return result;
        }
    };

    template <typename R, typename... Args>
    auto make_logger(R (*func)(Args...), const std::string &name)
    {
        return Logger<R(Args...)>(std::function<R(Args...)>(func), name);
    }

    double add(double a, double b)
    {
        return a + b;
    }

    void PassingFunction_to_ClassTemplateArgument()
    {
        auto logged_add = make_logger(add, "Add");
        auto result = logged_add(2, 3);
    }
}

void Templates::TestAll()
{
    // MathingTests();
    PassingFunction_to_ClassTemplateArgument();
}
