/**============================================================================
Name        : Monostate.cpp
Created on  : 04.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Monostate pattern src
============================================================================**/

#include "Monostate.h"

#include <iostream>
#include <mutex>
#include <thread>

namespace Monostate
{
    struct MonoConfig
    {
        MonoConfig()
        {
            // ensure a single initialization outside of the static chain if we don't
            // need multi-threaded safety we can downgrade to a boolean flag

            static std::once_flag onceFlag;
            std::call_once(onceFlag, initialize);

            // std::cout << "MonoConfig::MonoConfig()\n";
        }

        // Interface to access the monostate
        static uint32_t getValue() {
            return value;
        }

        static const std::string& getName() {
            return name;
        }

    private:

        static void initialize()
        {
            std::cout << "MonoConfig::initialize()\n";

            value = UINT32_C(42);
            name.assign("Hello World");
        }


        static inline uint32_t value {0};
        static inline std::string name;
    };
}

void Monostate::TestAll()
{

    MonoConfig a, b, c;
}

