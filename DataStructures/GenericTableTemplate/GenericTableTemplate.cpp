/**============================================================================
Name        : GenericTableTemplate.cpp
Created on  : 27.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : GenericTableTemplate.cpp
============================================================================**/

#include "GenericTableTemplate.h"

#include <iostream>
#include <string_view>
#include <tuple>
#include <vector>


template<typename ... Args>
struct GenericTable
{
    std::vector<std::tuple<Args...>> table {};

    template<typename ... Types>
    auto emplace(Types&& ... params) -> decltype(auto)
    {
        return table.emplace_back(std::forward<Args>(params) ...);
    }

    template <size_t N = 0, typename... Ts>
    constexpr void print(std::tuple<Ts...> tup)
    {
        if constexpr (N < sizeof...(Ts)) {
            std::cout << get<N>(tup) << "  ";
            print<N+1>(tup);
        }
    }

    void print()
    {
        for (const auto& row: table)
        {
            print(row);
            std::cout << std::endl;
        }
    }
};




void GenericTableTemplate::TestAll()
{

    {
        GenericTable<int, char, std::string> table;

        table.emplace(1, '1', std::string{"I"});
        table.emplace(2, '2', std::string{"II"});
        table.emplace(3, '3', std::string{"III"});

        table.print();
    }

    {
        GenericTable<float, std::string> table;

        table.emplace(1.9, std::string{"I"});
        table.emplace(2.1, std::string{"II"});
        table.emplace(3.2, std::string{"III"});

        table.print();
    }
}