//============================================================================
// Name        : Semaphore.h
// Created on  : 12.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Semaphore
//============================================================================

#include "SharedMemory.h"
#include "../common.h"

#include <iostream>
#include <string_view>
#include <thread>

#define INVALID_HANDLE  (-1)

namespace {
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_OBJECT_1KD8dkDLK343jhz" };
}

namespace SharedMemory
{

    struct SharedData {
    public:
        int handle {0};
        size_t counter {0};
    };

    void ReportError(std::string_view errText) {
        std::cout << errText << ". Error = " << errno << std::endl;
    }

    void InitSharedMem_Sleep_AndCloseSegment(size_t secondsToSleep = 30)
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                  O_CREAT|O_RDWR|O_EXCL|O_TRUNC, S_IRWXU|S_IRWXG);
        if (INVALID_HANDLE == sharedMemory) {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedMemory = ::shm_open(sharedMemoryObjName.data(), O_EXCL|O_RDWR, S_IRWXU|S_IRWXG);
                std::cout << "Open existing memory" << std::endl;
            } else
                ReportError("Failure on shm_open");
        } else {
            if (INVALID_HANDLE == ftruncate(sharedMemory, sizeof(SharedData))) {
                ReportError("Error on ftruncate()");
            }
        }

        SharedData* data = (SharedData*)::mmap(nullptr,
                                               sizeof(SharedData),
                                               PROT_READ | PROT_WRITE, MAP_SHARED,
                                               sharedMemory,
                                               0);
        data->handle = 474;
        data->counter++;

        std::cout << data->counter << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds (secondsToSleep));

        if (0 != close(sharedMemory))
            ReportError("Failed to close() shared memory");
        if (0 != shm_unlink(sharedMemoryObjName.data()))
            ReportError("Failed to shm_unlink() shared memory");
    }

    void GetDataFromSharedMemory_Test() {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),O_RDWR , S_IRWXU | S_IRWXG);
        if (sharedMemory < 0) {
            ReportError("Failure on shm_open");
            return;
        }
        if (-1 == ftruncate(sharedMemory, sizeof(SharedData))) {
            ReportError("Error on ftruncate()");
            return;
        }


        void* sharedBuffer = (SharedData*)::mmap(nullptr, sizeof(SharedData),
                                                 PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
        if (MAP_FAILED == sharedBuffer){
            ReportError("mmap() failed");
            return;
        }


        SharedData* data = reinterpret_cast<SharedData*>(sharedBuffer);
        std::cout << data->counter << std::endl;

        data->counter++;


        /*
        int mode = atoi(argv[1]);
        if (1 == mode)
        {
            int val;
            while (true) {
                std::cout << "Enter value : ";
                std::cin >> *i;
            }
        } else if (2 == mode) {
            while (true) {
                if (prev != *i) {
                    std::cout << *i << std::endl;
                    prev = *i;
                }
                nanosleep(&ts, &tr);
            }
        } else if (3 == mode) {
            munmap(sharedBuffer, SHARED_MEMORY_OBJECT_SIZE);
            close(sharedMemory);
            shm_unlink(SHARED_MEMORY_OBJECT_NAME);
        }
        */


        /*
        if (0 != close(sharedMemory))
            ReportError("Failed to close() shared memory");
        if (0 != shm_unlink(sharedMemoryObjName.data()))
            ReportError("Failed to shm_unlink() shared memory");
        */
    }
}

void SharedMemory::TestAll(const std::vector<std::string_view>& params)
{
    // InitSharedMem_Sleep_AndCloseSegment(10);

    GetDataFromSharedMemory_Test();
};

