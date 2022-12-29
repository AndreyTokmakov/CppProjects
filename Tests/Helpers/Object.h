/**============================================================================
Name        : Object.h
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Object
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_OBJECT_H
#define CPPPROJECTS_UTILITIES_OBJECT_H

#include <iostream>

namespace Utilities
{
    struct Object
    {
        Object()
        {
            std::cout << "Object()\n";
        }

        Object( [[maybe_unused]] const Object& obj)
        {
            std::cout << "Object(const Object& obj)\n";
        }

        Object& operator=([[maybe_unused]] const Object& obj)
        {
            std::cout << "Object& operator=(const Object& obj)\n";
            return *this;
        }

        Object([[maybe_unused]] Object&& obj) noexcept
        {
            std::cout << "Object(Object&& obj) noexcept\n";
        }

        Object& operator=([[maybe_unused]] Object&& obj) noexcept
        {
            std::cout << "Object& operator=(Object&& obj)\n";
            return *this;
        }

        ~Object()
        {
            std::cout << "~Object()\n";
        }
    };

}

#endif //CPPPROJECTS_UTILITIES_OBJECT_H