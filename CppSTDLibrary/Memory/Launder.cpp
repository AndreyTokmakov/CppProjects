/**============================================================================
Name        : Launder.cpp
Created on  : 22.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Launder.cpp
============================================================================**/

#include "Launder.hpp"

#include <cstdint>
#include <memory>
#include <iostream>

namespace Launder::Examples
{
    struct Object
    {
        int value { 0 };
    };

    void simpleExample()
    {
        using Type = int32_t;
        alignas(Type) uint8_t buffer[sizeof(Type)];

        Type* ptr = new (buffer) Type(111);
        ptr->~Type();

        ptr = new (buffer) Type(222);

        Type* ptr2 = std::launder(reinterpret_cast<Type*>(buffer));
        // Type* ptr2 = reinterpret_cast<Type*>(buffer);

        std::cout << *ptr2 << std::endl;
    }

    void simpleExample2()
    {
        alignas(Object) std::byte buffer[sizeof(Object)];
        Object* ptr = new(buffer) Object {2};

        {
            // Class member access is undefined behavior: reinterpret_cast<Y*>(&s)
            // has value "pointer to s" and does not point to a Y object
            const int val = reinterpret_cast<Object*>(&buffer)->value;
            std::cout << val << std::endl;
        }

        {
            // OK
            const int val = std::launder(reinterpret_cast<Object*>(&buffer))->value;
            std::cout << val << std::endl;
        }
    }
}


void Launder::TestAll()
{
    // Examples::simpleExample();
    Examples::simpleExample2();
}
