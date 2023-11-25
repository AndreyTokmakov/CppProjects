/**============================================================================
Name        : Concepts.cpp
Created on  : 18.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Concepts.cpp
============================================================================**/

#include "Concepts.h"

#include <iostream>

namespace Concepts
{
    template<typename T>
    concept SupportsValidation = requires(T t)
    {
        t.validate();
    };

    template<typename T>
    void Send(const T& data)
    {
        if constexpr(SupportsValidation<T>) {
            data.validate();
        }
        else {
            std::cout << "Can not be validated\n";
        }
    }

    struct EmptyObject { };

    struct Validator
    {
        void validate() const {
            std::cout << "ComplexType::validate()" << std::endl;
        }
    };

    void If_Constexpr_Concepts()
    {
        EmptyObject obj1;
        Validator obj2;

        Send(obj1);
        Send(obj2);

        static_assert(SupportsValidation<Validator>);
        static_assert(not SupportsValidation<EmptyObject>);
    }
}

namespace Concepts::FoldExpression
{
    template<typename T, typename ... Types>
    concept SameAsAnyOf = (... or std::same_as<T, Types>);

    template<typename ... Ts>
    struct Keeper
    {
        constexpr explicit Keeper(SameAsAnyOf<Ts ...  > auto obj) {
        }

        constexpr void setValue(SameAsAnyOf<Ts ...  > auto obj) {
        }
    };

    struct A {};
    struct B {};
    struct C {};

    void CheckThatTypeSameAs()
    {
        Keeper<int, double> keeper(1);
        Keeper<int, std::string> keeper1(std::string {"text"});

        Keeper<A, B> keeper3 { A{}};

        // Keeper<A, B> keeper4 { C{}};  // Shall not compile
    }
}

void Concepts::TestAll()
{
    // If_Constexpr_Concepts();

    FoldExpression::CheckThatTypeSameAs();
}