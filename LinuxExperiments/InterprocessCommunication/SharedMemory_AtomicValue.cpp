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
#include <utility>

#include "../common.h"

#define RESULT_OK       ( 0)
#define INVALID_HANDLE  (-1)

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

namespace
{
    constexpr std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_00000__2__" };

    int error(const std::string &func) {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }
}


namespace SharedMemoryUtilities
{
    struct SharedDataHeader
    {
        uint32_t useCount { 0 };
    };

    struct SharedDataBlock
    {
        SharedDataHeader header { 0 };
        // std::atomic<uint64_t> someTestCounter { 0 };
        uint64_t someTestCounter { 0 };
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        SharedDataBlock* sharedDataBlock { nullptr };

        inline uint32_t incrementUseCount() noexcept {
            return ++(sharedDataBlock->header.useCount);
        }

        inline uint32_t decrementUseCount() noexcept {
            return sharedDataBlock->header.useCount == 0 ? 0 : --(sharedDataBlock->header.useCount);
        }

        ~SharedData()
        {
            if (INVALID_HANDLE == handle || nullptr == sharedDataBlock)
                return;
            if (0 == decrementUseCount())
            {
                std::cout << "Closing shared memory [handle: " << handle << ", name: " << sharedSegmentName << "]\n";
                if (RESULT_OK != ::close(handle)) {
                    error("close()");
                }

                std::cout << "Unlink shared memory segment " << sharedSegmentName << std::endl;
                if (RESULT_OK != ::shm_unlink(sharedSegmentName.data())) {
                    error("shm_unlink()");
                }
            }
        }

        SharedData() = default;

        SharedData(const SharedData&) = delete;
        SharedData(SharedData&& sharedData) noexcept :
                handle { std::exchange(sharedData.handle, INVALID_HANDLE) },
                sharedDataBlock { std::exchange(sharedData.sharedDataBlock, nullptr) } {
        }

        SharedData& operator=(const SharedData&) = delete;
        SharedData& operator=(SharedData&&) noexcept = delete;
    };

    SharedData createSharedMemSegment()
    {
        SharedData sharedData;
        sharedData.handle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE != sharedData.handle)
        {
            const int retCode = ::ftruncate(sharedData.handle, sizeof(SharedDataBlock));
            ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");
        }
        else
        {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedData.handle= ::shm_open(sharedSegmentName.data(),O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
            } else { // TODO: Use std::source_location
                throw std::runtime_error("shm_open() failed. Error = " + std::to_string(errno));
            }
        }
        ASSERT_NOT(INVALID_HANDLE, sharedData.handle, "shm_open");
        return sharedData;
    }

    SharedData createSharedMapping()
    {
        SharedData sharedData = createSharedMemSegment();
        void *area = ::mmap(nullptr,
                            sizeof(SharedDataBlock),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            sharedData.handle,
                            0);
        ASSERT_NOT(MAP_FAILED, area, "mmap");

        sharedData.sharedDataBlock = reinterpret_cast<SharedDataBlock*>(area);
        sharedData.incrementUseCount();

        ASSERT_NOT(nullptr, sharedData.sharedDataBlock, "reinterpret_cast<SharedDataBlock*>(area)");
        return sharedData;
    }
}

#if 0
namespace SharedMemory_AtomicValue::Basic
{
    using ObjectType = int32_t;

    void Create()
    {
        int sharedMemory = ::shm_open(sharedSegmentName.data(),
                                      O_CREAT|O_RDWR|O_EXCL|O_TRUNC, S_IRWXU|S_IRWXG);
        if (INVALID_HANDLE == sharedMemory)
        {
            if (EEXIST == errno)
            {
                /** Shared memory already exist. **/
                sharedMemory = ::shm_open(sharedSegmentName.data(), O_EXCL|O_RDWR, S_IRWXU|S_IRWXG);
                std::cout <<  "Main  Process: Open existing memory" << std::endl;
            } else
                error("Failure on shm_open");
        }
        else {
            std::cout <<  "Main  Process: " << sharedSegmentName <<  "segment is created. Descriptor = "
                      << sharedMemory << std::endl;
        }

        if (INVALID_HANDLE == ::ftruncate(sharedMemory, sizeof(ObjectType))) {
            error("Error on ftruncate()");
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

        *value = 1122;

        std::this_thread::sleep_for(std::chrono::seconds (1));
        CloseSharedSegment(sharedMemory);
    }

    void Read()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (250));
        int sharedMemory = ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE == sharedMemory) {
            return error("shm_open()");
        } else {
            std::cout <<  "Child Process: " << sharedSegmentName <<  "segment is opened.  Descriptor = "
                      << sharedMemory << std::endl;
        }

        ObjectType* value = (ObjectType*)::mmap(nullptr,
                                                sizeof(ObjectType),
                                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                                sharedMemory,
                                                0);

       std::cout << "Child Process: Value = " << *value << std::endl;
    }

    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            Read();
        }
        else if (pid > 0) { /** Parent **/
            Create();
        }
        else {
            std::cout << "Unable to create child process" << std::endl;
        }
    }
};
#endif


uint64_t testsCount = 500'000'000;


namespace SharedMemory_AtomicValue::Atomic
{
    using namespace SharedMemoryUtilities;

    void ProcParent()
    {
        SharedData data = createSharedMapping();
        for (uint64_t i = 0; i < testsCount; ++i) {
            ++data.sharedDataBlock->someTestCounter;
        }

        std::this_thread::sleep_for(std::chrono::seconds (1));
        std::cout << data.sharedDataBlock->someTestCounter << std::endl;
    }

    void ProcChild()
    {
        SharedData data = createSharedMapping();
        for (uint64_t i = 0; i < testsCount; ++i) {
            ++data.sharedDataBlock->someTestCounter;
        }

        std::this_thread::sleep_for(std::chrono::seconds (1));
        std::cout << data.sharedDataBlock->someTestCounter << std::endl;
    }

    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            ProcChild();
        }
        else if (pid > 0) { /** Parent **/
            ProcParent();
        }
        else {
            std::cout << "Unable to create child process" << std::endl;
        }

        std::cout << "Done" << std::endl;
    }
};

void SharedMemory_AtomicValue::TestAll()
{
    // using namespace Basic;
    using namespace Atomic;

    // Create();
    // Read();
    MultiProcessTest();
};