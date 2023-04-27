/**============================================================================
Name        : Random.cpp
Created on  : 25.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Random
============================================================================**/

#include "Random.h"

#include <iostream>
#include "Poco/Random.h"
#include "Poco/RandomStream.h"

namespace Random
{
    using Poco::Random;
    using Poco::RandomInputStream;

    void Test()
    {
        Random rnd;
        rnd.seed();
        std::cout << "Random integer: " << rnd.next() << std::endl;
        std::cout << "Random digit: " << rnd.next(10) << std::endl;
        std::cout << "Random char: " << rnd.nextChar() << std::endl;
        std::cout << "Random bool: " << rnd.nextBool() << std::endl;
        std::cout << "Random double: " << rnd.nextDouble() << std::endl;
        RandomInputStream ri;
        std::string rs;
        ri >> rs;
    }
}

void Random::TestAll()
{
    Test();
}