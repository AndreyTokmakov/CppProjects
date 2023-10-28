/**============================================================================
Name        : Atomic_IsLockFree.cpp
Created on  : 28.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Atomic_IsLockFree.cpp
============================================================================**/

#include "Atomic_IsLockFree.h"

#include <iostream>
#include <atomic>

namespace IsLockFree_Types
{
    void ByteAlignedTypes()
    {
        struct ByteStruct {
            uint8_t value;
        };

        struct TwoBytesStruct {
            uint8_t value;
            uint8_t value2;
        };

        struct FourBytesStruct {
            uint8_t value;
            uint8_t value2;
            uint8_t value3;
            uint8_t value4;
        };

        std::atomic<ByteStruct> byteAtomic;
        std::atomic<TwoBytesStruct> twoBytesAtomic;
        std::atomic<FourBytesStruct> fourBytesAtomic;
        std::atomic<uint64_t> bigAtomic;

        std::cout << "bigAtomic is_lock_free = " << std::boolalpha << bigAtomic.is_lock_free() << std::endl;
        std::cout << "byteAtomic is_lock_free = " << std::boolalpha << byteAtomic.is_lock_free() << std::endl;
        std::cout << "twoBytesAtomic is_lock_free = " << std::boolalpha << twoBytesAtomic.is_lock_free() << std::endl;
        std::cout << "fourBytesAtomic is_lock_free = " << std::boolalpha << fourBytesAtomic.is_lock_free() << std::endl;

        /// bigAtomic is_lock_free = true
        /// byteAtomic is_lock_free = true
        /// twoBytesAtomic is_lock_free = true
        /// fourBytesAtomic is_lock_free = true
    }

    void OddBytesTypes()
    {
        struct ThreeBytesStruct {
            uint8_t value;
            uint8_t value2;
            uint8_t value3;
        };
        std::atomic<ThreeBytesStruct> threeBytesAtomic;

        // std::cout << "threeBytesAtomic is_lock_free = " << std::boolalpha << threeBytesAtomic.is_lock_free() << std::endl;

        /// ERROR: Atomic_IsLockFree.cpp:(.text+0x2d8): undefined reference to `__atomic_is_lock_free'
    }
}

void Atomic_IsLockFree::TestAll()
{
    // IsLockFree_Types::ByteAlignedTypes();
    IsLockFree_Types::OddBytesTypes();
}