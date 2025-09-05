/**============================================================================
Name        : ScopeExit.cpp
Created on  : 05.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ScopeExit.cpp
============================================================================**/

#include "ScopeExit.hpp"

#include <print>
#include <iostream>
#include <experimental/scope>

namespace ScopeExit::Basics
{
    void callOnExit()
    {
        std::experimental::scope_exit ok ( [&](){ std::println("ON_EXIT"); } );
        std::println("Doing something");
    }

}

namespace ScopeExit::UseCase
{
    void delete_backup()
    {
        std::println("Deleting BackUp");
    }

    void restore_backup()
    {
        std::println("Deleting Restoring");
    }

    void modify(int value)
    {
        if (value < 0) {
            std::cerr << "Value is negative\n";
            throw std::runtime_error("Can not write negative value");
        }

        std::cout << "Success: New value is " << value << std::endl;
    }

    void update_db(const int value)
    {
        std::experimental::scope_exit ok ( [&](){ delete_backup(); } );
        std::experimental::scope_fail failure ( [&](){ restore_backup(); } );

        modify(value);

        // failure.release();
        ok.release();
    }

    void UpdateData()
    {
        try {
            // update_db(10);
            update_db(-10);
        }
        catch (const std::exception& exc) {
            std::cerr << exc.what() << std::endl;
        }
    }
}


void ScopeExit::TestAll()
{
    Basics::callOnExit();
    // UseCase::UpdateData();
}
