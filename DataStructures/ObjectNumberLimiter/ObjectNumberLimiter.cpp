/**============================================================================
Name        : ObjectNumberLimiter.cpp
Created on  : 10.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObjectNumberLimiter
============================================================================**/

#include "ObjectNumberLimiter.h"

#include <iostream>
#include <atomic>

namespace ObjectNumberLimiter
{
    template <class LimitedClassType, uint32_t maxInstance>
    struct LimitNoOfInstances
    {
        static inline std::atomic<uint32_t> count = 0;

        LimitNoOfInstances() {
            if (count >= maxInstance)
                throw std::logic_error { std::string {"Too Many Instances of class "}
                    .append(typeid(LimitedClassType).name())};
            ++count;
        }

        ~LimitNoOfInstances() {
            --count;
        }
    }; // Copy, move & other sanity checks to be complete


    void Test()
    {
        struct One : LimitNoOfInstances<One, 1> { };
        struct Two : LimitNoOfInstances<Two, 2> { };

        try {
            Two one, two;
        } catch (std::exception &e) {
            std::cout << "TWO: " << e.what() << std::endl;
        }

        try {
            One one, two;
        } catch (std::exception &e) {
            std::cout << "ONE: " << e.what() << std::endl;
        }
    }
}

void ObjectNumberLimiter::TestAll()
{
    Test();
}