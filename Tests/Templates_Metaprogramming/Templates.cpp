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
#include <functional>
#include <utility>
#include <cstdint>

namespace Templates::FoldExpressions
{

    template <typename R, typename ... Ts>
    constexpr auto matches(const R& range, Ts ... ts) -> decltype(auto) {
        return (std::count(std::begin(range), std::end(range), ts) + ...);
    }

    void MatchingTests()
    {
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

        [[maybe_unused]]
        auto result = logged_add(2, 3);
    }
}


namespace Templates::NTTP
{
    struct Config
    {
        int v{12};
    };

    template<Config config>
    struct Task
    {
        void submit() {
            std::cout << config.v << std::endl;
        }
    };

    template<auto Func>
    struct PersonalBudget {
        double compute(std::uint32_t amt) {
            return Func(amt);
        }
    };



    void testConfig() {
        Task<Config{}>().submit();
    }

    void testPersonalBudget ()
    {
        auto savings = [](int amt) -> decltype(auto) {
            return static_cast<double>(0.75*amt);
        };

        PersonalBudget<savings> savingsBudget{};

        auto saveResult = savingsBudget.compute(2300);
        std::cout << "Estimated Savings: " << saveResult << std::endl;
    }
}


namespace Templates
{
    /*
    template<typename T, typename ... Args>
    T (*Func)(Args ...) = [](Args ... args) {
        return (args + ... + 0);
    };
    */

    class Customer
    {
    private:
        std::string name;
    public:
        explicit Customer(std::string n) : name(std::move(n)) {
        }

        [[nodiscard]]
        std::string getName() const { return name; }
    };

    struct CustomerEq {
        bool operator() (Customer const& c1, Customer const& c2) const {
            std::cout << "CustomerEq() called" << std::endl;
            return c1.getName() == c2.getName();
        }
    };

    struct CustomerHash {
        std::size_t operator() (Customer const& c) const {
            std::cout << "CustomerHash() called" << std::endl;
            return std::hash<std::string>()(c.getName());
        }
    };

    template<typename... Bases>
    struct Overloader : Bases...
    {
        using Bases::operator()...; // OK since C++17
    };

    void Test2()
    {
        using CustomerOP = Overloader<CustomerHash,CustomerEq>;

        const Customer c1 {"one"}, c2 { "two"};

        CustomerOP{}(c1);
        CustomerOP{}(c1, c2);
    }
}


void Templates::TestAll()
{
    // FoldExpressions::MatchingTests();
    // FoldExpressions::PassingFunction_to_ClassTemplateArgument();

    NTTP::testConfig();
    NTTP::testPersonalBudget();
}
