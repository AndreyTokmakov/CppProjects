/**============================================================================
Name        : ConstexprMap.cpp
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : ConstexprMap
============================================================================**/

#include "ConstexprMap.h"

#include <array>
#include <string_view>
#include <algorithm>

namespace ConstexprMap
{
    enum class Method {
        None = 0,
        GET = 1,
        POST = 2,
        HEAD = 3,
        PUT = 4,
        // DELETE = 5,
        CONNECT = 6,
        OPTIONS = 7,
        TRACE = 8,
        PATCH = 9
    };

    using HTTPMethoDefinition = std::pair<std::string_view, Method>;
    static constexpr std::array<HTTPMethoDefinition, 8> TABLE {{
        { std::string_view("GET"),     Method::GET },
        { std::string_view("POST"),    Method::POST },
        { std::string_view("HEAD"),    Method::HEAD },
        { std::string_view("PUT"),     Method::PUT },
        // { std::string_view("DELETE"),  Method::DELETE },
        { std::string_view("CONNECT"), Method::CONNECT },
        { std::string_view("OPTIONS"), Method::OPTIONS },
        { std::string_view("TRACE"),   Method::TRACE },
        { std::string_view("PATCH"),   Method::PATCH }
    }};

    constexpr auto getEntry(const std::string_view methodName) {
        return std::find_if(TABLE.cbegin(), TABLE.cend(),
                            [&](const auto& entry) { return entry.first == methodName; });
    }
};

void ConstexprMap::TestAll()
{
    static_assert(Method::GET   == getEntry("GET")->second);
    static_assert(Method::HEAD  == getEntry("HEAD")->second);
    static_assert(Method::POST  == getEntry("POST")->second);
    static_assert(Method::PATCH == getEntry("PATCH")->second);
};