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
#include <array>
#include <csignal>

#define INVALID_HANDLE  (-1)


namespace SharedMemory::DemoOne
{
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_OBJECT_1KD8dkDLK343jhz" };

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

    void GetDataFromSharedMemory_Test()
    {
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


namespace SharedMemory::DemoTwo
{
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_OBJECT_00000001" };

    struct Data
    {
        uint32_t size {0};
        std::array<char, 1024> buffer {};
    };

    void error(const std::string& func)
    {
        std::cerr << func << " filed. Error = " << errno << std::endl;
    }

    void CreateAndCloseSharedSegment()
    {
        int sharedMemory = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT|O_RDWR|O_EXCL|O_TRUNC, S_IRWXU|S_IRWXG);
        if (INVALID_HANDLE == sharedMemory) {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedMemory = ::shm_open(sharedMemoryObjName.data(), O_EXCL|O_RDWR, S_IRWXU|S_IRWXG);
                std::cout << "Open existing memory" << std::endl;
            } else {
                error("shm_open()");
            }
        } else {
            std::cout << sharedMemoryObjName << " segment is opened" << std::endl;
        }

        [[maybe_unused]]
        void* area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            sharedMemory,
                            0);


        std::this_thread::sleep_for(std::chrono::seconds (3));

        if (0 != close(sharedMemory)) {
            error("close()");
        } else {
            std::cout << sharedMemory << " handle is closed" << std::endl;
        }

        if (0 != shm_unlink(sharedMemoryObjName.data())) {
            error("shm_unlink()y");
        } else {
            std::cout << sharedMemoryObjName << " segment is removed" << std::endl;
        }
    }

    int CreateSharedSegment()
    {
        int sharedHandle = ::shm_open(sharedMemoryObjName.data(),
                                      O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == sharedHandle) {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedHandle = ::shm_open(sharedMemoryObjName.data(), O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
                std::cout << "Open existing memory. Descriptor = " << sharedHandle << std::endl;
            } else {
                error("shm_open()");
            }
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << sharedHandle << std::endl;
        }

        return sharedHandle;
    }

    void WriteSharedData(int sharedHandle)
    {
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

        dataPtr->size = 16;
        memcpy(dataPtr->buffer.data(), "1111111111111111111111111111", dataPtr->size);

    }

    void ReadeSharedData(int sharedHandle)
    {
        void *area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ, MAP_SHARED,
                            sharedHandle,
                            0);
        if (MAP_FAILED == area) {
            error("mmap()");
        }

        Data* dataPtr = reinterpret_cast<Data*>(area);
        if (!dataPtr) {
            std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
        }

       std::cout << "\tsize  : " << dataPtr->size << std::endl;
       std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;

    }

    void CloseSharedSegment(int sharedHandle)
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
}

namespace SharedMemory::DemoThree
{
    constexpr std::string_view sharedMemoryObjName{"__SHARED_MEMORY_OBJECT_00000001"};

    struct Data
    {
        uint32_t size{0};
        std::array<char, 1024> buffer{};
    };

    void error(const std::string &func) {
        std::cerr << func << " failed. Error = " << errno << std::endl;
    }

    int sharedHandle = -1;

    void signalHandler(int /* sigId */)
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

        std::exit(0);
    };

    void CreateSegmentProcess()
    {
        sharedHandle = ::shm_open(sharedMemoryObjName.data(),
                                  O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
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

        dataPtr->size = 16;
        memcpy(dataPtr->buffer.data(), "1111111111111111111111111111", dataPtr->size);

        std::this_thread::sleep_for(std::chrono::seconds(60));
        signalHandler(0);
    }

    void ReadeSharedDataProcess()
    {
        int handle = ::shm_open(sharedMemoryObjName.data(),
                                O_RDWR, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == handle) {
            error("shm_open()");
            return;
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << handle << std::endl;
        }

        void *area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ, MAP_SHARED,
                            handle,
                            0);
        if (MAP_FAILED == area) {
            error("mmap()");
        }

        Data* dataPtr = reinterpret_cast<Data*>(area);
        if (!dataPtr) {
            std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
        }

        std::cout << "\tsize  : " << dataPtr->size << std::endl;
        std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;
    }
}


namespace SharedMemory::DemoFour
{
    constexpr std::string_view sharedMemoryObjName{"__SHARED_MEMORY_OBJECT_00000003"};

    struct Data
    {
        uint32_t size{0};
        std::array<char, 1024> buffer{};
    };

    int error(const std::string &func) {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    int sharedHandle = -1;

    void signalHandler([[maybe_unused]] int sigId)
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

        std::exit(0);
    };

    void CreateAndReadSegmentProcess()
    {
        sharedHandle = ::shm_open(sharedMemoryObjName.data(),
                                  O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
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

        while (true)
        {
            std::cout << "\tsize  : " << dataPtr->size << std::endl;
            std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    void WriteSharedDataProcess()
    {
        int handle = ::shm_open(sharedMemoryObjName.data(),
                                O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == handle) {
            error("shm_open()");
            return;
        } else {
            std::cout << sharedMemoryObjName << " segment is opened. Descriptor = " << handle << std::endl;
        }

        void *area = ::mmap(nullptr,
                            sizeof(Data),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            handle,
                            0);
        if (MAP_FAILED == area) {
            error("mmap()");
        }

        Data* dataPtr = reinterpret_cast<Data*>(area);
        if (!dataPtr) {
            std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
        }


        dataPtr->size = 22;
        memcpy(dataPtr->buffer.data(), "1111111111111111111111111111", dataPtr->size);

    }
};

void SharedMemory::TestAll(const std::vector<std::string_view>&)
{
    // DemoOne::InitSharedMem_Sleep_AndCloseSegment(10);
    // DemoOne:: GetDataFromSharedMemory_Test();

    // DemoTwo::CreateAndCloseSharedSegment();
    /*
    int handle = DemoTwo::CreateSharedSegment();
    DemoTwo::WriteSharedData(handle);
    DemoTwo::ReadeSharedData(handle);
    DemoTwo::CloseSharedSegment(handle);
    */

    // DemoThree::CreateSegmentProcess();
    // DemoThree::ReadeSharedDataProcess();

    // DemoFour::CreateAndReadSegmentProcess();
    DemoFour::WriteSharedDataProcess();
};

