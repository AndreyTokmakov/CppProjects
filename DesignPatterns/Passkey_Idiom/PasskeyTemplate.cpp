/**============================================================================
Name        : PasskeyTemplate.cpp
Created on  : 09.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PasskeyTemplate.cpp
============================================================================**/

#include "PasskeyIdiom.hpp"

#include <iostream>
#include <ostream>

namespace
{
    template <typename T>
    class Passkey
    {
        friend T;
        Passkey() = default;
    };

    struct ConnectionManagerOne;
    struct ConnectionManagerTwo;

    struct Connection
    {
        void open(Passkey<ConnectionManagerOne>) {
        }
    };

    struct ConnectionManagerOne
    {
        void manage(Connection& c) {
            c.open(Passkey<ConnectionManagerOne>{});
        }
    };

    struct ConnectionManagerTwo
    {
        void manage([[maybe_unused]] Connection& c)
        {
            // Will not compile
            // c.open(Passkey<ConnectionManagerTwo>{});
        }
    };
}

void demo()
{

}