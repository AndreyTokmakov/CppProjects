/**============================================================================
Name        : SharedMemory_PyExchange.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemory_PyExchange.cpp
============================================================================**/

#include "SharedMemory_PyExchange.h"


#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <utility>
#include <random>
#include <format>
#include "../common.h"


#define RESULT_OK (0)
#define RESULT_FAILURE (-1)
#define INVALID_HANDLE  (-1)

#define LOG std::cout << now()
#define ERR std::cerr << now()

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

namespace SharedMemory_PyExchange
{
    using namespace std::chrono;

        constexpr std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_00000__2__" };
    constexpr std::string_view FORMAT { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };

    [[nodiscard]]
    std::string now(const time_point<system_clock>& timestamp = system_clock::now()) noexcept
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(&(buffer.front()), FORMAT.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }

    int error(const std::string &func)
    {
        ERR << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    struct SemaphoreGuard
    {
        sem_t* semRelease { nullptr };

        SemaphoreGuard(sem_t* semWait, sem_t* semRelease):
                semRelease { semRelease }
        {
            ::sem_wait(semWait);
        }

        ~SemaphoreGuard() {
            ::sem_post(semRelease);
        }
    };

    struct SharedDataBlock
    {
        uint32_t useCount { 0 };
        uint32_t dataSize { 0 };
        std::array<uint8_t, 1024> data {};
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        void* mappedArea { nullptr };
        SharedDataBlock* sharedDataBlock { nullptr };

        inline uint32_t incrementUseCount() const noexcept {
            // return sharedDataBlock->header.useCount.fetch_add(1, std::memory_order_relaxed) + 1;
            return ++sharedDataBlock->useCount;
        }

        inline uint32_t decrementUseCount() const noexcept {
            // return sharedDataBlock->header.useCount.fetch_sub(1, std::memory_order_relaxed) - 1;
            return --sharedDataBlock->useCount;
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
            std::cout << "Block created: " << sharedData.handle << std::endl;
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
            std::cout << "Block opened: " << sharedData.handle << std::endl;
        }
        ASSERT_NOT(INVALID_HANDLE, sharedData.handle, "shm_open");

        sharedData.allocateShared();

        std::cout << "DEBUG:  [count:" << sharedData.sharedDataBlock->useCount
                  << ", handle: " << sharedData.handle
                  << ", sharedDataBlock: " << sharedData.mappedArea << "]\n";
        return sharedData;
    }
}


namespace SharedMemory_PyExchange::Tests
{
    void Create_Close_SharedMemory()
    {
        try {
            SharedData data = getSharedData();

            data.sharedDataBlock->dataSize = 6;
            strcpy(reinterpret_cast<char *>(data.sharedDataBlock->data.data()), "QWERTY");

            std::this_thread::sleep_for(std::chrono::seconds(5U));

            std::cout << data.sharedDataBlock->dataSize << std::endl;
            std::cout << std::string_view {reinterpret_cast<const char *>(data.sharedDataBlock->data.data()),
                                           data.sharedDataBlock->dataSize } << std::endl;
        }
        catch (const std::exception& exc) {
            std::cerr << exc.what() << std::endl;
        }

    }
}


void SharedMemory_PyExchange::TestAll(const std::vector<std::string_view>&)
{
    Tests::Create_Close_SharedMemory();
}
