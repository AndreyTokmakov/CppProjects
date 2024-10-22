/**============================================================================
Name        : SharedBlock_WithSemaphore.cpp
Created on  : 22.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedBlock_WithSemaphore.cpp
============================================================================**/

#include "SharedBlock_WithSemaphore.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <utility>
#include <random>
#include "../common.h"

#define RESULT_OK       ( 0)
#define INVALID_HANDLE  (-1)

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

#define LOG std::cout << now()
#define ERR std::cerr << now()

namespace
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

    [[nodiscard]]
    std::string randomString(size_t size = 32)
    {
        std::random_device rd{};
        std::mt19937 generator = std::mt19937 {rd()};
        auto ud = std::uniform_int_distribution<> {(int)'a', (int)'z'};

        std::string str;
        str.reserve(size);
        while (size-- > 0)
            str.push_back(static_cast<char>(ud(generator)));
        return str;
    }
}


namespace
{
    // TODO: Rename to Header
    struct SharedDataBlock
    {
        static inline constexpr uint16_t semaphoreNameSize { 32 };

        std::atomic<int32_t> useCount { 0 };
        std::array<char, 32> semaphoreName {};
    };

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        sem_t* semaphore { nullptr };
        std::string semaphoreName {}; // TODO: Remove ???
        SharedDataBlock* sharedDataBlock { nullptr };

        [[nodiscard]]
        inline uint32_t incrementUseCount() const noexcept
        {
            const uint32_t count = sharedDataBlock->useCount.load();
            LOG << "incrementUseCount = " << count << " --> " << count + 1 << std::endl;

            return sharedDataBlock->useCount.fetch_add(1) + 1;
        }

        [[nodiscard]]
        inline uint32_t decrementUseCount() const noexcept
        {
            const uint32_t count = sharedDataBlock->useCount.fetch_sub(1) - 1;
            LOG << "decrementUseCount = " << count  + 1 << " --> " << count << std::endl;

            return count;
        }

        void initSemaphore()
        {
            semaphoreName.assign(randomString(SharedDataBlock::semaphoreNameSize));
            memcpy(sharedDataBlock->semaphoreName.data(), semaphoreName.data(), SharedDataBlock::semaphoreNameSize);
            semaphore = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
            ASSERT_NOT(SEM_FAILED, semaphore, "sem_open");

            LOG << "Semaphore [name: " << semaphoreName << "] created\n";
        }

        void openSemaphore()
        {
            semaphoreName.assign(sharedDataBlock->semaphoreName.data(), SharedDataBlock::semaphoreNameSize);
            semaphore = ::sem_open(semaphoreName.data(), O_CREAT);
            ASSERT_NOT(SEM_FAILED, semaphore, "sem_open");

            LOG << "Semaphore [name: " << semaphoreName << "] opened\n";
        }

        void closeSemaphore() const noexcept
        {
            LOG << "Closing semaphore [name: " << semaphoreName << "]\n";
            if (0 != ::sem_close(semaphore)) {
                error("sem_close()");
            }
            if (0 != ::sem_unlink(semaphoreName.data())) {
                error("sem_unlink()");
            }
        }

        ~SharedData()
        {
            if (INVALID_HANDLE == handle || nullptr == sharedDataBlock)
                return;
            if (0 == decrementUseCount())
            {
                closeSemaphore(); // TODO: Check result

                LOG << "Delete memory mapping [" << sharedDataBlock << "]\n";
                if (RESULT_OK != ::munmap(sharedDataBlock, sizeof(SharedDataBlock))) {
                    error("munmap()");
                }

                LOG << "Closing shared memory [handle: " << handle << ", name: " << sharedSegmentName << "]\n";
                if (RESULT_OK != ::close(handle)) {
                    error("close()");
                }

                LOG << "Unlink shared memory segment " << sharedSegmentName << std::endl;
                if (RESULT_OK != ::shm_unlink(sharedSegmentName.data())) {
                    error("shm_unlink()");
                }
            }
        }

        SharedData() = default;

        SharedData(const SharedData&) = delete;
        SharedData(SharedData&& sharedData) noexcept :
                handle { std::exchange(sharedData.handle, INVALID_HANDLE) },
                semaphore { std::exchange(sharedData.semaphore, nullptr) },
                semaphoreName { std::move(sharedData.semaphoreName) },
                sharedDataBlock { std::exchange(sharedData.sharedDataBlock, nullptr) } {
        }

        SharedData& operator=(const SharedData&) = delete;
        SharedData& operator=(SharedData&&) noexcept = delete;
    };

    SharedData createSharedMemSegment()
    {
        SharedData sharedData;
        /// Both O_CREAT and O_EXCL were specified to shm_open() and the shared memory object specified by name already exists.
        sharedData.handle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
        if (INVALID_HANDLE != sharedData.handle)
        {
            const int retCode = ::ftruncate(sharedData.handle, sizeof(SharedDataBlock));
            ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");
            LOG << "Shared memory [handle: " << sharedData.handle << ", name: " << sharedSegmentName << "] created\n";
        }
        else
        {
            if (EEXIST == errno) { /** Shared memory already exist. **/
                sharedData.handle= ::shm_open(sharedSegmentName.data(),
                                              O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
            } else { // TODO: Use std::source_location
                throw std::runtime_error("shm_open() failed. Error = " + std::to_string(errno));
            }
            LOG << "Shared memory [handle: " << sharedData.handle << ", name: " << sharedSegmentName << "] opened\n";
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

        LOG << "Memory mapping [" << area << "] created\n";
        sharedData.sharedDataBlock = reinterpret_cast<SharedDataBlock *>(area);
        const uint32_t useCount = sharedData.incrementUseCount();

        if (1 == useCount) {
            sharedData.initSemaphore(); // TODO: Check result
        } else {
            sharedData.openSemaphore(); // TODO: Check result
        }

        ASSERT_NOT(nullptr, sharedData.sharedDataBlock, "reinterpret_cast<SharedDataBlock*>(area)");
        return sharedData;
    }
}

namespace Create_Delete_Multiprocess_Test
{
    void ProcParent()
    {
        SharedData data = createSharedMapping();
        std::this_thread::sleep_for(std::chrono::microseconds (200U));
    }

    void ProcChild()
    {
        std::this_thread::sleep_for(std::chrono::microseconds (1U));
        SharedData data = createSharedMapping();
        std::this_thread::sleep_for(std::chrono::microseconds (100U));
    }

    void Test()
    {
        if (const pid_t pid = fork(); pid == 0) /** Child **/
            ProcChild();
        else if (pid > 0) /** Parent **/
            ProcParent();
        else
            ERR<< "Unable to create child process" << std::endl;
        LOG << "Done" << std::endl;
    }
};

namespace Wait_Semaphore_Multiprocess_Test
{
    void Parent_WaitSemaphore()
    {
        SharedData data = createSharedMapping();
        sem_wait(data.semaphore);
    }

    void Child_ReleaseSemaphore()
    {
        // std::this_thread::sleep_for(std::chrono::microseconds (1U));
        SharedData data = createSharedMapping();
        std::this_thread::sleep_for(std::chrono::seconds (1U));
        sem_post(data.semaphore);
    }

    void Test()
    {
        if (const pid_t pid = fork(); pid == 0)
            Child_ReleaseSemaphore(); /** Child **/
        else if (pid > 0)
            Parent_WaitSemaphore();   /** Parent **/
        else
            ERR << "Unable to create child process" << std::endl;
        LOG << "Done" << std::endl;
    }
};


void SharedBlock_WithSemaphore::TestAll([[maybe_unused]] const std::vector<std::string_view> &params)
{
    // Create_Delete_Multiprocess_Test::Test();
    Wait_Semaphore_Multiprocess_Test::Test();

    // SharedData data = createSharedMapping();
    // std::this_thread::sleep_for(std::chrono::seconds (std::atoi(params.front().data())));
}