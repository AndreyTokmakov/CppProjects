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

void Concepts::TestAll()
{
    If_Constexpr_Concepts();
}