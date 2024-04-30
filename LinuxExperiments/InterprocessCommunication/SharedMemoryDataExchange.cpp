/**============================================================================
Name        : SharedMemoryDataExchange.cpp
Created on  : 30.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchange.cpp
============================================================================**/

#include "SharedMemoryDataExchange.h"

#include "SharedMemory.h"
#include "../common.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <csignal>


#define INVALID_HANDLE  (-1)


namespace SharedMemoryDataExchange
{
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_OBJECT_00000002"};
    constexpr std::string_view semaphoreName { "my_named_semaphore_test" };

    int sharedHandle = -1;

    struct Data
    {
        uint32_t size{0};
        std::array<char, 1024> buffer{};
    };

    void error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
    }

    void closeSemaphore(sem_t *sem)
    {
        if (sem_close(sem) < 0) {
            std::cout << "sem_close() failed. Error = " << errno << std::endl;
        } else {
            std::cout << "Semaphore closed" << std::endl;
        }

        if (sem_unlink(semaphoreName.data()) < 0) {
            std::cout << "sem_unlink() failed. Error = " << errno << std::endl;
        } else {
            std::cout << "Semaphore unlinked" << std::endl;
        }
    }

    void closeSharedMem()
    {
        if (0 != close(sharedHandle)) {
            error("close()");
        } else {
            std::cout << sharedHandle << " handle is closed" << std::endl;
        }

        if (0 != shm_unlink(sharedMemoryObjName.data())) {
            error("shm_unlink()");
        } else {
            std::cout << sharedMemoryObjName << " segment is removed" << std::endl;
        }
    }

    void signalHandler(int sigId)
    {
        closeSharedMem();
        std::exit(0);
    };

    void Consumer()
    {
        sharedHandle = ::shm_open(sharedMemoryObjName.data(),
                                  O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == sharedHandle) {
            error("shm_open()");
            return;
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << sharedHandle << std::endl;
        }

        signal(SIGINT , signalHandler);
        signal(SIGTERM , signalHandler);
        signal(SIGQUIT , signalHandler);
        signal(SIGKILL , signalHandler);
        signal(SIGPIPE , signalHandler);

        const int retCode = ftruncate(sharedHandle, sizeof(Data));
        if (INVALID_HANDLE == retCode) {
            error("ftruncate");
        } else {
            std::cout << "Segment '" << sharedHandle << "' has been truncated to " << sizeof(Data) << " bytes\n";
        }

        void *area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            sharedHandle,
                            0);
        if (MAP_FAILED == area) {
            error("mmap()");
        }

        Data* dataPtr = reinterpret_cast<Data*>(area);
        if (!dataPtr) {
            std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
        }

        sem_t *sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
        if (SEM_FAILED == sem) {
            error("sem_open()");
            signalHandler(0); // FIXME
            return;
        }

        // timespec timeout {10, 0};
        while (true)
        {
            std::cout << "Waiting for semaphore...." << std::endl;

            //  const int result = sem_timedwait(sem, &timeout);
            int result = sem_wait(sem);

            std::cout << "Released" << std::endl;

            std::cout << "\tsize  : " << dataPtr->size << std::endl;
            std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;
        }
    }

    void Producer()
    {
        int handle = ::shm_open(sharedMemoryObjName.data(),
                                O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == handle) {
            error("shm_open()");
            return;
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << handle << std::endl;
        }

        /*
        const int retCode = ftruncate(sharedHandle, sizeof(Data));
        if (INVALID_HANDLE == retCode) {
            error("ftruncate");
        } else {
            std::cout << "Segment '" << sharedHandle << "' has been truncated to " << sizeof(Data) << " bytes\n";
        }*/

        void *area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            handle,
                            0);
        if (MAP_FAILED == area) {
            error("mmap()");
        }

        std::cout << "----------------------\n";


        Data* dataPtr = reinterpret_cast<Data*>(area);
        if (!dataPtr) {
            std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
        }

        std::cout << "----------------------\n";

        dataPtr->size = 16;
        memcpy(dataPtr->buffer.data(), "1111111111111111111111111111", dataPtr->size);

        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            error("sem_getvalue");
        } else {
            std::cout << "Semaphore is taken" << std::endl;
        }

        //int value {0};
        //int result = sem_getvalue(sem,&value);

        sem_post(sem);
        std::cout << "Semaphore is released." << std::endl;
    }
}

void SharedMemoryDataExchange::TestAll(const std::vector<std::string_view> &params)
{
    // Consumer();
    Producer();
}