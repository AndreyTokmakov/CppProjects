/**============================================================================
Name        : SharedMemoryWrapper.cpp
Created on  : 07.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryWrapper.cpp
============================================================================**/

#include "SharedMemoryWrapper.h"


#include <iostream>
#include <string_view>
#include <thread>
#include <optional>
#include <format>
#include <chrono>
#include <utility>

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define RESULT_OK (0)
#define RESULT_FAILURE (-1)
#define INVALID_HANDLE  (-1)

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

constexpr std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_0000012__" };


namespace SharedMemoryWrapper
{
    int error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    struct SharedDataHeader
    {
        std::atomic<uint32_t> useCount { 0 };
    };

    struct SharedDataBlock
    {
        SharedDataHeader header { 0 };
        uint64_t someTestCounter { 0 };
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        void* mappedArea { nullptr };
        SharedDataBlock* sharedDataBlock { nullptr };

        inline uint32_t incrementUseCount() const noexcept {
            return sharedDataBlock->header.useCount.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        inline uint32_t decrementUseCount() const noexcept {
            return sharedDataBlock->header.useCount.fetch_sub(1, std::memory_order_relaxed) - 1;
        }

        // TODO: Rename
        void allocateShared()
        {
            /** Create Memory mapping **/
            mappedArea = ::mmap(nullptr,
                                sizeof(SharedDataBlock),
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                handle,
                                0);
            ASSERT_NOT(MAP_FAILED, mappedArea, "mmap");

            sharedDataBlock = reinterpret_cast<SharedDataBlock*>(mappedArea);
            ASSERT_NOT(nullptr, sharedDataBlock, "reinterpret_cast<SharedDataBlock*>(area)");

            incrementUseCount();
        }

        void deallocate() const
        {
            std::cout << "\tDeallocate mapping [area: " << mappedArea << ", size: " << sizeof(SharedDataBlock) << "]\n";
            if (RESULT_OK != ::munmap(mappedArea, sizeof(SharedDataBlock))) {
                error("munmap()");
            }
        }

        void closeAndUnlink() const
        {
            std::cout << "\tClosing shared memory [handle: " << handle << ", name: " << sharedSegmentName << "]\n";
            if (RESULT_OK != ::close(handle)) {
                error("close()");
            }

            std::cout << "\tUnlink shared memory segment " << sharedSegmentName << std::endl;
            if (RESULT_OK != ::shm_unlink(sharedSegmentName.data())) {
                error("shm_unlink()");
            }
        }

        ~SharedData()
        {
            if (INVALID_HANDLE == handle || nullptr == sharedDataBlock)
                return;
            const uint32_t count = decrementUseCount();
            std::cout << "~SharedData() [count:" << count << ", handle: " << handle
                      << ", sharedDataBlock: " << sharedDataBlock << "]\n";
            if (0 == count)
            {
                deallocate();
                closeAndUnlink();
                handle = INVALID_HANDLE;
                sharedDataBlock = nullptr;
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


    SharedData getSharedData()
    {
        SharedData sharedData;
        sharedData.handle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE != sharedData.handle)
        {
            const int retCode = ::ftruncate(sharedData.handle, sizeof(SharedDataBlock));
            ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate")
        }
        else
        {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedData.handle = ::shm_open(sharedSegmentName.data(),O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
            } else { // TODO: Use std::source_location
                throw std::runtime_error("shm_open() failed. Error = " + std::to_string(errno));
            }
        }
        ASSERT_NOT(INVALID_HANDLE, sharedData.handle, "shm_open");

        sharedData.allocateShared();
        std::cout << "DEBUG:  [count:" << sharedData.sharedDataBlock->header.useCount.load(std::memory_order::relaxed)
                  << ", handle: " << sharedData.handle
                  << ", sharedDataBlock: " << sharedData.mappedArea << "]\n";
        return sharedData;
    }
}


namespace SharedMemoryWrapper::Tests
{
    void Create_And_Close_SingleProcess()
    {
        SharedData data1 = getSharedData();

        ++data1.sharedDataBlock->someTestCounter;
        std::cout << "Test counter: " << data1.sharedDataBlock->someTestCounter << std::endl;

        SharedData data2 = getSharedData();

        ++data1.sharedDataBlock->someTestCounter;
        std::cout << "Test counter: " << data1.sharedDataBlock->someTestCounter << std::endl;

        SharedData data3 = getSharedData();

        ++data1.sharedDataBlock->someTestCounter;
        std::cout << "Test counter: " << data1.sharedDataBlock->someTestCounter << std::endl;
    }

    void Atomics()
    {
        std::atomic<uint32_t> counter {0};
        std::cout << counter.fetch_add(1, std::memory_order_relaxed) << std::endl;
        std::cout << counter.load(std::memory_order_relaxed) << std::endl;
        std::cout << counter.fetch_sub(1, std::memory_order_relaxed) << std::endl;
        std::cout << counter.load(std::memory_order_relaxed) << std::endl;
    }
}

void SharedMemoryWrapper::TestAll()
{

    Tests::Create_And_Close_SingleProcess();
    // Tests::Atomics();
}