/**============================================================================
Name        : Environment.cpp
Created on  : 25.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Environment
============================================================================**/

#include "Environment.h"

#include <iostream>
#include "Poco/Environment.h"

namespace Environment
{
    using Poco::Environment;

    void printOS_Info()
    {
        std::cout
                << "OS Name: " << Environment::osName() << std::endl
                << "OS Version: " << Environment::osVersion() << std::endl
                << "OS Arch: " << Environment::osArchitecture() << std::endl
                << "Node Name: " << Environment::nodeName() << std::endl
                << "Node ID: " << Environment::nodeId() << std::endl;

        if (Environment::has("HOME"))
            std::cout << "Home: " << Environment::get("HOME") << std::endl;

        Environment::set("POCO", "foo");
    }
}


void Environment::TestAll()
{
    printOS_Info();
}