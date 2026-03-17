/**============================================================================
Name        : Metaprogramming.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Metaprogramming.cpp
============================================================================**/

#include "Templates.hpp"

#include <tuple>
#include <utility>
#include <iostream>
#include <print>

namespace metaprogramming
{
    template<typename Ty>
    struct Keeper
    {
        const Ty& value;

        template<typename U>
        explicit Keeper(U&& obj) : value { std::forward<U>(obj) }
        {
            static_assert(!std::reference_constructs_from_temporary_v<const Ty&,U>, "Error: Dangling reference");
        }
    };


    void Constructs_from_Temporary_Check()
    {
        {
            std::string lvalueStr {"Some data" };
            Keeper<std::string> lvalueKeeper {lvalueStr};
        }

#if 0
        {
            Keeper<std::string> lvalueKeeper {"Some data"};
        }
        // Error: static assertion failed: Error: Dangling reference
        //  static_assert(!std::reference_constructs_from_temporary_v<const Ty&,U>, "Error: Dangling reference");

#endif
    }
}


void Templates::Metaprogramming::TestAll()
{
    metaprogramming::Constructs_from_Temporary_Check();
}
