/**============================================================================
Name        : SharedMemory_AtomicValue.cpp
Created on  : 29.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemory_AtomicValue.cpp
============================================================================**/

#include "SharedMemory_AtomicValue.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <csignal>

#include "../common.h"

#define INVALID_HANDLE  (-1)

namespace
{
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_ATOMIC_OBJECT__1__" };

    void error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
    }

    void CloseSharedSegment(int sharedHandle)
    {
        if (0 != ::close(sharedHandle)) {
            error("close()");
        } else {
            std::cout << sharedHandle << " handle is closed" << std::endl;
        }

        if (0 != ::shm_unlink(sharedMemoryObjName.data())) {
            error("shm_unlink()");
        } else {
            std::cout << sharedMemoryObjName << " segment is removed" << std::endl;
        }
    }
}

namespace SharedMemory_AtomicValue::Basic
{
    using ObjectType = int32_t;

    void Create()
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT|O_RDWR|O_EXCL|O_TRUNC, S_IRWXU|S_IRWXG);
        if (INVALID_HANDLE == sharedMemory)
        {
            if (EEXIST == errno)
            {
                /** Shared memory already exist. **/
                sharedMemory = ::shm_open(sharedMemoryObjName.data(), O_EXCL|O_RDWR, S_IRWXU|S_IRWXG);
                std::cout << "Open existing memory" << std::endl;
            } else
                error("Failure on shm_open");
        }

        if (INVALID_HANDLE == ::ftruncate(sharedMemory, sizeof(ObjectType))) {
            error("Error on ftruncate()");
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

        *value = 1234567;

        std::this_thread::sleep_for(std::chrono::seconds (10));

        CloseSharedSegment(sharedMemory);
    }

    void Read()
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == sharedMemory) {
            return error("shm_open()");
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << sharedMemory << std::endl;
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

       std::cout << *value << std::endl;
    }
};

namespace SharedMemory_AtomicValue::Atomic
{
    using ObjectType = std::atomic<int>;

    void Create()
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT|O_RDWR|O_EXCL|O_TRUNC, S_IRWXU|S_IRWXG);
        if (INVALID_HANDLE == sharedMemory)
        {
            if (EEXIST == errno)
            {
                /** Shared memory already exist. **/
                sharedMemory = ::shm_open(sharedMemoryObjName.data(), O_EXCL|O_RDWR, S_IRWXU|S_IRWXG);
                std::cout << "Open existing memory" << std::endl;
            } else
                error("Failure on shm_open");
        }

        if (INVALID_HANDLE == ::ftruncate(sharedMemory, sizeof(ObjectType))) {
            error("Error on ftruncate()");
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

        value->store(10);

        std::this_thread::sleep_for(std::chrono::seconds (5));

        std::cout << "changing to 123" << std::endl;
        value->store(123);

        std::this_thread::sleep_for(std::chrono::seconds (5));

        CloseSharedSegment(sharedMemory);
    }

    void Read()
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == sharedMemory) {
            return error("shm_open()");
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << sharedMemory << std::endl;
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

        const int32_t currentValue = value->load(std::memory_order_relaxed);
        std::cout << currentValue << std::endl;

        std::cout << "waiting ....." << std::endl;
        value->wait(currentValue);
        std::cout << "Done: " << value->load(std::memory_order_relaxed) << std::endl;
    }
};

void SharedMemory_AtomicValue::TestAll()
{
    // using namespace Basic;
    using namespace Atomic;

    Create();
    // Read();
};