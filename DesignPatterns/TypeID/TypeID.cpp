/**============================================================================
Name        : TypeID.cpp
Created on  : 12.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeID.cpp
============================================================================**/

#include "TypeID.h"

#include <iostream>
#include <atomic>
#include <string_view>

namespace TypeID
{
    struct TypeIdCnt
    {
        template<typename>
        static uint32_t GetUniqueId()
        {
            static const uint32_t TypeId = NewTypeId();
            return TypeId;
        }

    private:

        static uint32_t NewTypeId()
        {
            static std::atomic<uint32_t> CurrentId = 0;
            return CurrentId++;
        }
    };

    template<typename T>
    static uint32_t GetTypeId()
    {
        return TypeIdCnt::GetUniqueId<T>();
    }
}

namespace TypeID::Tests
{
    struct A {};
    struct B {};
    struct C: A {};


}

void TypeID::TestAll()
{
    using namespace Tests;

    std::cout << "int   = " << GetTypeId<int>() << std::endl;
    std::cout << "short = " << GetTypeId<short>() << std::endl;
    std::cout << "char  = " << GetTypeId<char>() << std::endl;
    std::cout << "int   = " << GetTypeId<int>() << std::endl;
    std::cout << "A     = " << GetTypeId<A>() << std::endl;
    std::cout << "B     = " << GetTypeId<B>() << std::endl;
    std::cout << "C     = " << GetTypeId<C>() << std::endl;
    std::cout << "A     = " << GetTypeId<A>() << std::endl;
}