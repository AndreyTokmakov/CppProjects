//============================================================================
// Name        : PocoProjects.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : PocoProjects C++
//============================================================================

#include <string_view>
#include <vector>

#include "DataTime/Timer.h"
#include "Foundation/Events.h"
#include "Processes/Processes.h"
#include "Networking/Networking.h"
#include "Environment/Environment.h"
#include "Random/Random.h"
#include "Cryptography/Cryptography.h"

#include "Poco/JSONString.h"



namespace Experiments
{


}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Timer::TestAll();
    // Events::TestAll();
    // Processes::TestAll();
    // Networking::TestAll();
    Environment::TestAll();
    // Random::TestAll();
    // Cryptography::TestAll();


    return EXIT_SUCCESS;
}

