/**============================================================================
Name        : RefQualifierMemberFunctions.cpp
Created on  : 28.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RefQualifierMemberFunctions.cpp
============================================================================**/

#include "RefQualifierMemberFunctions.h"

#include <iostream>
#include <utility>
#include <vector>

namespace RefQualifierMemberFunctions
{
    struct FooBar
    {
        void func() &&{
            std::cout << "FooBar::func() && " << std::endl;
        }
    };

    void SimpleTest()
    {
        auto a = FooBar{};
        //a.func();           // Does not compile, 'a' is not an r-value
        std::move(a).func();  // Compiles
        FooBar{}.func();      // Compiles
    }
}

namespace RefQualifierMemberFunctions
{
    struct String
    {
        std::string value;

        std::string &get_value() &
        {
            std::puts("get_value() &");
            return value;
        }

        std::string get_value() &&
        {
            std::puts("get_value() &&");
            return std::move(value);
        }

        [[nodiscard]]
        const std::string &get_value() const &
        {
            std::puts("get_value() const &");
            return value;
        }

        [[nodiscard]]
        const std::string &get_value() const &&
        {
            std::puts("get_value() const &&");
            return value;
        }
    };

    [[nodiscard]]
    String getString() {
        return {};
    }

    void RefQualifier_ConstObjectCall()
    {
        const String obj;
        std::move(obj).get_value();
    }
};

namespace RefQualifierMemberFunctions
{
    class Keeper
    {
        std::vector<int> data{2, 3, 4};

    public:
        Keeper() {
            std::cout << "Keeper::Keeper()\n";
        }

        ~Keeper() {
            std::cout << "Keeper::~Keeper()\n";
        }

        auto &items() &{
            return data;
        }

        // INFO: From a performance point of view, you might see an unnecessary copy in Keeper::data.
        //       The compiler isn't able to implicitly move the return value here.
        //       It needs a little help from us.
        auto items() &&{
            // return data;  // OK - but move is better
            return std::move(data);
        }
    };

    void GoodExample()
    {
        {
            auto k = Keeper();
            for (auto &item: k.items())
                std::cout << item << std::endl;
        }
        std::cout << "--------------------------------------------------\n";
        {
            for (auto &item: Keeper().items())
                std::cout << item << std::endl;
        }
    }
}

void RefQualifierMemberFunctions::TestAll()
{
    // SimpleTest();
    // RefQualifier_ConstObjectCall(); // --->  output:  'get_value() const &&'
    GoodExample();
};