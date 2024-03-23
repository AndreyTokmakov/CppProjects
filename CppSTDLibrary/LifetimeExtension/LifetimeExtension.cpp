/**============================================================================
Name        : LifetimeExtension.cpp
Created on  : 23.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LifetimeExtension.cpp
============================================================================**/

#include "LifetimeExtension.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace LifetimeExtension
{
    struct Name
    {
        std::string value;

        explicit Name(std::string str): value(std::move(str)) {
            std::cout << "Name(" << value << ")\n";
        }

        ~Name() {
            std::cout << "~Name(" << value << ")\n";
        }
    };

    struct User
    {
        Name name;
        std::vector<int> tokens;

        explicit User(std::string str): name(std::move(str)) {
            std::cout << "User(" << name.value << ", tokens: " << tokens.size() << ")\n";
        }

        User(std::string str, std::vector<int> values):
            name {std::move(str)}, tokens { std::move(values) }  {
            std::cout << "User(" << name.value << ", tokens: " << tokens.size() << ")\n";
        }

        ~User() {
            std::cout << "~User(" << name.value << ", tokens: " << tokens.size() << ")\n";
        }
    };

    [[nodiscard]]
    User get_user()
    {
        return {"Dmitry",{1,2,3,4,5}};
    }
}


namespace LifetimeExtension::ReferenceToObjectField
{
    void Test()
    {
        std::string&& name = "";

        {
            std::cout << "-------- Entering scope --------" << std::endl;
            name = get_user().name.value;
            std::cout << "-------- Exiting scope  --------" << std::endl;
        }

        std::cout << "Exit function\n";
    }
}


void LifetimeExtension::TestAll()
{
    // User user {"Jonh Dow"};

    ReferenceToObjectField::Test();

}