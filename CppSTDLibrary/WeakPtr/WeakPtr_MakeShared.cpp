/**============================================================================
Name        : WeakPtr_MakeShared.cpp
Created on  : 22.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : WeakPtr_MakeShared.cpp
============================================================================**/

#include "WeakPtr_MakeShared.h"

#include <iostream>
#include <memory>
#include <string>
#include <new>


void* operator new(size_t count)
{
    auto * ptr = malloc(count);
    std::cout << "allocating " << count << " bytes at " << ptr << '\n';
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    std::cout << "deallocating " << ptr << "\n";
    free(ptr);
}

namespace
{
    struct Object
    {
        std::array<int, 16> buffer {};
    };

    void WeakPtr_OutLive_SharedPtr__NoLeak()
    {
        std::cout << std::endl << std::endl;
        {
            std::weak_ptr<Object> weakPtr;
            {
                std::shared_ptr<Object> sharedPtr(new Object);
                weakPtr = sharedPtr;
                std::cout << "End of sharedPtr" << std::endl;
            }
            std::cout << "End of weakPtr" << std::endl;
        }
        std::cout << std::endl << std::endl;

        /**
        allocating 64 bytes at 0xefa07f0
        allocating 24 bytes at 0xefa0840
        End of sharedPtr
        deallocating 0xefa07f0
        End of weakPtr
        deallocating 0xefa0840
        **/
    }

    void WeakPtr_OutLive_SharedPtr__MemLeak()
    {
        std::cout << std::endl << std::endl;
        {
            std::weak_ptr<Object> weakPtr;
            {
                std::shared_ptr<Object> sharedPtr = std::make_shared<Object>();
                weakPtr = sharedPtr;
                std::cout << "End of sharedPtr" << std::endl;
            }
            std::cout << "End of weakPtr" << std::endl;
        }
        std::cout << std::endl << std::endl;

        /**
        allocating 80 bytes at 0x157e77f0
        End of sharedPtr
        End of weakPtr
        deallocating 0x157e77f0
        **/
    }
}


void WeakPtr_MakeShared::TestAll()
{
    WeakPtr_OutLive_SharedPtr__NoLeak();
    // WeakPtr_OutLive_SharedPtr__MemLeak();
}