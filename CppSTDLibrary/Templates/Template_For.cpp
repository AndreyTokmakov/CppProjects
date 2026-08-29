/**============================================================================
Name        : Template_For.cpp.cpp
Created on  : 14.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PerfectForwarding
============================================================================**/

#include "Templates.hpp"

#include <iostream>
#include <print>
#include <string>
#include <utility>

namespace
{
    template <int Number>
    void print_my_number()
    {
        std::cout << "My number = " << Number << "\n";
    }

    // variable v has to bee usable as the non-type template argument to the print_my_number<>.

    // template for generates the necessary instantiation of the print_my_number<>
    // template function using the loop iterator v and that solves our earlier problem.
    void template_for_use_case()
    {
        static constexpr std::array<int, 3> my_array{ 1, 2, 3 };
        template for (constexpr int v : my_array)
        {
            print_my_number<v>();
        }
    }


}


void Templates::template_for::TestAll()
{
    template_for_use_case();
}
