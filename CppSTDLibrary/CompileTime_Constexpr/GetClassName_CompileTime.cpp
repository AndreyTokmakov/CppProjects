/**============================================================================
Name        : GetClassName_CompileTime.cpp
Created on  : 31.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : GetClassName_CompileTime.cpp
============================================================================**/

#include "GetClassName_CompileTime.hpp"

#include <string_view>

struct MyStruct
{
    [[nodiscard]]
    static constexpr std::string_view getClassName() noexcept
    {
        constexpr std::string_view name { __PRETTY_FUNCTION__ };
        constexpr auto end = name.find_last_of(':');
        constexpr auto start = name.rfind(' ', end);
        return name.substr(start + 1, end - start - 2);
    }
};


void GetClassName_CompileTime::TestAll()
{
    static_assert("MyStruct" == MyStruct::getClassName());
}