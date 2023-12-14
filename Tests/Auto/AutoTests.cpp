/**============================================================================
Name        : AutoTests.cpp
Created on  : 26.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CollectionsTests
============================================================================**/

#include "AutoTests.h"

#include <iostream>

namespace AutoTests
{
    struct Object
    {
        Object() {
            //std::cout << "Object::Object()" << std::endl;
        }

        ~Object() {
            //std::cout << "~Object::Object()" << std::endl;
        }

        Object(const Object&) { std::cout << "Object::Object(const Object&)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object::Object(Object&&)" << std::endl; }

        Object& operator=(const Object&) {
            std::cout << "Object::Object(const Object&)" << std::endl;
            return *this;
        }

        Object& operator=(Object&&) noexcept {
            std::cout << "Object::Object(Object&&)" << std::endl;
            return *this;
        }
    };

    Object tmp;

    Object& getObject() {
        return tmp;
    }

    void Test_GetReference()
    {
        [[maybe_unused]] Object& obj = getObject();
    }

    void Test_LoseReference_Copy()
    {
        [[maybe_unused]] auto obj = getObject();
    }
}

void AutoTests::TestAll()
{
    Test_GetReference();
    Test_LoseReference_Copy();
};