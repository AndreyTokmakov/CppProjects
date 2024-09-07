/**============================================================================
Name        : SemMutex.cpp
Created on  : 31.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SemMutex.cpp
============================================================================**/

#include "SemMutex.h"

#include "../common.h"
#include <semaphore.h>

#include <iostream>
#include <string_view>
#include <thread>
#include <optional>
#include <format>
#include <utility>


#include <chrono>


namespace
{

#define RESULT_OK (0)
#define RESULT_FAILURE (-1)
#define INVALID_HANDLE  (-1)

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

    constexpr std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_0000012__" };
    constexpr std::string_view semaphoreName { "Read_Semaphore_Name_One__1" };

    int error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    struct SharedDataHeader
    {
        std::atomic<uint32_t> useCount { 1 };
        uint64_t someTestCounter { 0 };
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        SharedDataHeader* header { nullptr };

        ~SharedData()
        {
            if (INVALID_HANDLE == handle || nullptr == header)
                return;
            const uint32_t count = header->useCount.fetch_sub(1);
            std::cout << "Usage count = " << count << std::endl;
            if (1 == count)
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
            header { std::exchange(sharedData.header, nullptr) } {
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
            const int retCode = ::ftruncate(sharedData.handle, sizeof(SharedDataHeader));
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
                            sizeof(SharedDataHeader),
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            sharedData.handle,
                            0);
        ASSERT_NOT(MAP_FAILED, area, "mmap");

        sharedData.header = reinterpret_cast<SharedDataHeader*>(area);

        sharedData.header->useCount.fetch_add(1);

        // TODO: Remove
        ASSERT_NOT(nullptr, sharedData.header, "reinterpret_cast<Data*>(area)");
        return sharedData;
    }

    std::optional<sem_t*> createSemaphore()
    {
        sem_t *sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 1);
        if (SEM_FAILED == sem) {
            error("sem_open()");
            return std::nullopt;
        }
        return sem;
    }

    std::optional<sem_t*> openSemaphore()
    {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            error("sem_open");
            return std::nullopt;
        }
        return sem;
    }

    struct LockGuard
    {
        sem_t *semaphore { nullptr };

        explicit LockGuard(sem_t* sem): semaphore {sem} {
            ::sem_wait(semaphore);
        }

        ~LockGuard() {
            ::sem_post(semaphore);
        }

        LockGuard(const LockGuard&) = delete;
        LockGuard(LockGuard&&) noexcept = delete;

        LockGuard& operator=(const LockGuard&) = delete;
        LockGuard& operator=(LockGuard&&) noexcept = delete;
    };

    void closeSemaphore(std::optional<sem_t*> sem)
    {
        if (!sem.has_value())
            return;

        if (0 != sem_close(sem.value())) {
            error("sem_close()");
        }
        if (0 != sem_unlink(semaphoreName.data())) {
            error("sem_unlink()");
        }

        std::cout << semaphoreName << " semaphore closed" << std::endl;
    }
}


namespace SemMutex::Tests
{
    void ProcParent()
    {
        SharedData data = createSharedMapping();
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
    }

    void ProcChild()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (1));
        SharedData data = createSharedMapping();
    }

    void MultiProcessSharedDataTest()
    {
        if (const pid_t pid = fork(); pid == 0)
            ProcChild();
        else if (pid > 0)
            ProcParent();
    }

    // uint64_t testsCount = 50'000'000'000;
    uint64_t testsCount = 10;

    void ChildProcess()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        std::optional<sem_t*> semaphore = openSemaphore();
        if (!semaphore) {
            std::cout << "ChildProcess: Failed to open semaphore" << std::endl;
            return;
        }

        SharedData data = createSharedMapping();

        for (uint64_t i = 0; i < testsCount; ++i)
        {
            LockGuard lock {semaphore.value()};
            ++data.header->someTestCounter;

            std::cout << data.header->someTestCounter << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (100));
        }

    }

    void ParentProcess()
    {
        std::optional<sem_t*> semaphore = createSemaphore();
        SharedData data = createSharedMapping();
        if (!semaphore) {
            std::cout << "ParentProcess: Failed to create semaphore" << std::endl;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds (10));

        for (uint64_t i = 0; i < testsCount; ++i)
        {
            LockGuard lock {semaphore.value()};
            ++data.header->someTestCounter;

            std::cout << data.header->someTestCounter << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (100));
        }


        closeSemaphore(semaphore);
        std::this_thread::sleep_for(std::chrono::seconds (2));

    }

    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            ChildProcess();
        } else if (pid > 0) { /** Parent **/
            ParentProcess();
        } else {
            std::cerr << "Unable to create child process" << std::endl;
        }
    }

}

void SemMutex::TestAll()
{
    Tests::MultiProcessTest();
    // Tests::MultiProcessSharedDataTest();

    // SharedData data = createSharedMapping();


    /*
    std::atomic<uint32_t> counter {0};
    std::cout << counter.fetch_add(1, std::memory_order_relaxed) << std::endl;
    std::cout << counter.load(std::memory_order_relaxed) << std::endl;
    std::cout << counter.fetch_sub(1, std::memory_order_relaxed) << std::endl;
    std::cout << counter.load(std::memory_order_relaxed) << std::endl;
    */
}