/**============================================================================
Name        : ClassTemplateArgumentDeduction.cpp
Created on  : 21.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ClassTemplateArgumentDeduction.cpp
============================================================================**/

#include "ClassTemplateArgumentDeduction.h"

#include <iostream>
#include <utility>
#include <vector>
#include <string_view>


namespace ClassTemplateArgumentDeduction::Hints
{
    template <typename T>
    struct Storage
    {
        explicit Storage(T obj) : value(std::move(obj)) {}

        template<std::input_iterator It>
        Storage(It begin, It end) : value(begin,end) {}

        T value;
    };

    // Constructor match -> Template instance
    Storage(const char*) -> Storage<std::string>;

    // Deduction guides can be templated
    template<std::input_iterator It>
    Storage(It,It) -> Storage<std::vector<typename std::iterator_traits<It>::value_type>>;

    void Test()
    {
        Storage a {10}; // decltype(a) == Storage<int> no guide required, deduced from Storage(T t)
        static_assert(std::is_same_v<decltype(a), Storage<int>>);

        Storage b {"Hello World!"};  // decltype(b) == Storage<std::string> follows the const char* guide
        static_assert(std::is_same_v<decltype(b), Storage<std::string>>);

        std::vector<int> data {1, 2, 3, 4, 5, 6};
        Storage c {data.begin(), data.end()}; // Storage<std::vector<int>> - follows the double iterator guide
        static_assert(std::is_same_v<decltype(c), Storage<std::vector<int>>>);
    }
}

void ClassTemplateArgumentDeduction::TestAll()
{
    Hints::Test();
};
