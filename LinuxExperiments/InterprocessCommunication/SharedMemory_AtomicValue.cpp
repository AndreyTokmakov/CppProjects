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
        //uint32_t someTestCounter { 0 };
        std::atomic<uint32_t> someTestCounter { 0 };
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        SharedDataBlock* sharedDataBlock { nullptr };

        uint32_t incrementUseCount() const noexcept {
            return ++(sharedDataBlock->header.useCount);
        }

        uint32_t decrementUseCount() const noexcept {
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

        std::cout << area << " of size " << sizeof(SharedDataBlock) << std::endl;

        sharedData.sharedDataBlock = static_cast<SharedDataBlock*>(area);
        sharedData.incrementUseCount();

        ASSERT_NOT(nullptr, sharedData.sharedDataBlock, "static_cast<SharedDataBlock*>(area)");
        return sharedData;
    }
}


namespace SharedMemory_AtomicValue::Atomic
{
    using namespace SharedMemoryUtilities;
    uint32_t testsCount = 1'000'000'000;

    void Create_Close_Test()
    {
        SharedData data = createSharedMapping();
        std::this_thread::sleep_for(std::chrono::seconds (1));
    }

    __attribute__((optimize("O0")))
    void ProcParent()
    {
        SharedData data = createSharedMapping();
        for (int n = 0; n < 1; ++n)
        {
            for (uint32_t i = 0; i < testsCount; ++i) {
                //++data.sharedDataBlock->someTestCounter;
                data.sharedDataBlock->someTestCounter.fetch_add(1, std::memory_order::relaxed);
            }
        }

        std::cout << data.sharedDataBlock->someTestCounter.load(std::memory_order::relaxed) << std::endl;
    }


    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            ProcParent();
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
    using namespace Atomic;

    // Create_Close_Test();
    // MultiProcessTest();

    ProcParent();

};