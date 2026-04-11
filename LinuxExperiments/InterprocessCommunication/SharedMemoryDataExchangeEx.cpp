/**============================================================================
Name        : SharedMemoryDataExchangeEx.cpp
Created on  : 22.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchangeEx.cpp
============================================================================**/

#include "SharedMemoryDataExchangeEx.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <utility>
#include <random>
#include <format>
#include "semaphore.h"
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
        static constexpr uint16_t semaphoreNameSize { 32 };

        std::atomic<int32_t> useCount { 0 };
        std::array<char, 32> semaphoreReadName {};
        std::array<char, 32> semaphoreWriteName {};

        uint32_t size { 0 };
        std::array<char, 1024 * 1024> buffer {};
    };

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

    struct SharedData
    {
        int32_t handle { INVALID_HANDLE };
        sem_t* semaphoreReader { nullptr };
        sem_t* semaphoreWriter { nullptr };
        std::string semaphoreNameReader {}; // TODO: Remove ???
        std::string semaphoreNameWriter {}; // TODO: Remove ???
        SharedDataBlock* sharedDataBlock { nullptr };

        [[nodiscard]]
        inline uint32_t incrementUseCount() const noexcept
        {
            return sharedDataBlock->useCount.fetch_add(1) + 1;
        }

        [[nodiscard]]
        inline uint32_t decrementUseCount() const noexcept
        {
            return sharedDataBlock->useCount.fetch_sub(1) - 1;
        }

        void initSemaphore()
        {
            semaphoreNameReader.assign(randomString(SharedDataBlock::semaphoreNameSize));
            memcpy(sharedDataBlock->semaphoreReadName.data(), semaphoreNameReader.data(), SharedDataBlock::semaphoreNameSize);
            semaphoreReader = ::sem_open(semaphoreNameReader.data(), O_CREAT, 0777, 0);
            ASSERT_NOT(SEM_FAILED, semaphoreReader, "sem_open");
            LOG << "Read semaphore [name: " << semaphoreNameReader << "] created\n";

            semaphoreNameWriter.assign(randomString(SharedDataBlock::semaphoreNameSize));
            memcpy(sharedDataBlock->semaphoreWriteName.data(), semaphoreNameWriter.data(), SharedDataBlock::semaphoreNameSize);
            semaphoreWriter = ::sem_open(semaphoreNameWriter.data(), O_CREAT, 0777, 1);
            ASSERT_NOT(SEM_FAILED, semaphoreWriter, "sem_open");
            LOG << "Write semaphore [name: " << semaphoreNameWriter << "] created\n";
        }

        void openSemaphore()
        {
            semaphoreNameReader.assign(sharedDataBlock->semaphoreReadName.data(), SharedDataBlock::semaphoreNameSize);
            semaphoreReader = ::sem_open(semaphoreNameReader.data(), O_CREAT);
            ASSERT_NOT(SEM_FAILED, semaphoreReader, "sem_open");
            LOG << "Read semaphore [name: " << semaphoreNameReader << "] opened\n";

            semaphoreNameWriter.assign(sharedDataBlock->semaphoreWriteName.data(), SharedDataBlock::semaphoreNameSize);
            semaphoreWriter = ::sem_open(semaphoreNameWriter.data(), O_CREAT);
            ASSERT_NOT(SEM_FAILED, semaphoreWriter, "sem_open");
            LOG << "Write semaphore [name: " << semaphoreNameWriter << "] opened\n";
        }

        void closeSemaphores() const noexcept
        {
            LOG << "Closing read semaphore [name: " << semaphoreNameReader << "]\n";
            if (0 != ::sem_close(semaphoreReader)) {
                error("sem_close()");
            }
            if (0 != ::sem_unlink(semaphoreNameReader.data())) {
                error("sem_unlink()");
            }

            // LOG << "Closing write semaphore [name: " << semaphoreNameWriter << "]\n";
            if (0 != ::sem_close(semaphoreWriter)) {
                error("sem_close()");
            }
            if (0 != ::sem_unlink(semaphoreNameWriter.data())) {
                error("sem_unlink()");
            }
        }

        ~SharedData()
        {
            if (INVALID_HANDLE == handle || nullptr == sharedDataBlock)
                return;
            if (0 == decrementUseCount())
            {
                closeSemaphores(); // TODO: Check result

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
                semaphoreReader { std::exchange(sharedData.semaphoreReader, nullptr) },
                semaphoreWriter { std::exchange(sharedData.semaphoreWriter, nullptr) },
                semaphoreNameReader { std::move(sharedData.semaphoreNameReader) },
                semaphoreNameWriter { std::move(sharedData.semaphoreNameWriter) },
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

namespace SharedMemoryDataExchangeEx
{
    void Parent_Consumer()
    {
        SharedData data = createSharedMapping();

        for (int i = 0; i < 5; ++i)
        {
            SemaphoreGuard guard {data.semaphoreReader, data.semaphoreWriter };
            LOG << "Processing data" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (500U));
        }
    }

    void Child_Producer()
    {
        SharedData data = createSharedMapping();

        for (int i = 0; i < 5; ++i)
        {
            SemaphoreGuard guard {data.semaphoreWriter, data.semaphoreReader };
            LOG << "Sending data" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (500U));
        }
    }

    void Test()
    {
        const pid_t pid = fork();
        if (0 == pid)
            Child_Producer(); /** Child **/
        else if (pid > 0)
            Parent_Consumer();   /** Parent **/
        else
            ERR << "Unable to create child process" << std::endl;

        if (0 == pid) {
            LOG << "* * * Done * * *" << std::endl;
        }
    }
};

namespace ExchangeMessages
{
    void ReadMessages(SharedData &sharedData)
    {
        uint32_t count = 0;
        while (true) {
            sem_wait(sharedData.semaphoreReader);
            std::string_view data { sharedData.sharedDataBlock->buffer.data(),
                                    sharedData.sharedDataBlock->size};
            std::cout << ++count << " | Data: " << data << std::endl;
            if ("quit" == data) {
                break;
            }
        }
    }

    void PutMessage(SharedData &sharedData,
                    std::string_view payload,
                    uint32_t count = 0)
   {
        for (uint32_t idx = 0; idx < count; ++idx) {
            sharedData.sharedDataBlock->size = payload.length();
            memcpy(sharedData.sharedDataBlock->buffer.data(), payload.data(), payload.size());
            sem_post(sharedData.semaphoreReader);
        }
    }

    /**
    > cp LinuxExperiments consume
    > cp LinuxExperiments produce
    > ./consume                       <--- Terminal 1
    > ./produce 1234523232 1          <--- Terminal 2
    **/

    void Producer_Consumer([[maybe_unused]] const std::vector<std::string_view> &params)
    {
        SharedData data = createSharedMapping();
        // ReadMessages(data);
        PutMessage(data, params.front(), std::atoi(params[1].data()));
    }
}

namespace ExchangeMessagesMultiprocess
{
    void Consumer ()
    {
        SharedData shData = createSharedMapping();
        uint32_t count = 0;
        while (true)
        {
            SemaphoreGuard guard { shData.semaphoreReader, shData.semaphoreWriter };
            const std::string data { shData.sharedDataBlock->buffer.data(),shData.sharedDataBlock->size };
            std::cout << ++count << " | Data: " << data << std::endl;
            if ("quit" == data) {
                break;
            }
        }
    }

    void Producer()
    {
        SharedData shData = createSharedMapping();

        std::string payload;
        for (uint32_t idx = 0; idx < 10'000; ++idx)
        {
            payload = std::format("Hello_{}", idx + 1);
            SemaphoreGuard guard { shData.semaphoreWriter, shData.semaphoreReader };

            shData.sharedDataBlock->size = payload.length();
            memcpy(shData.sharedDataBlock->buffer.data(), payload.data(), payload.size());
        }

        payload.assign("quit");
        shData.sharedDataBlock->size = payload.length();
        memcpy(shData.sharedDataBlock->buffer.data(), payload.data(), payload.size());
    }

    void MultiprocessTest()
    {
        const pid_t pid = fork();
        if (0 == pid)
            Producer(); /** Child **/
        else if (pid > 0)
            Consumer();  /** Parent **/
    }
};

namespace ExchangeMessagesMultiprocess_LoadTest
{

    void Consumer ()
    {
        SharedData shData = createSharedMapping();

        const high_resolution_clock::time_point start { high_resolution_clock::now() };
        uint64_t bytesRead = 0, count = 0;
        while (true)
        {
            SemaphoreGuard guard { shData.semaphoreReader, shData.semaphoreWriter };
            const std::string data { shData.sharedDataBlock->buffer.data(),shData.sharedDataBlock->size };

            bytesRead += shData.sharedDataBlock->size;
            ++count;
            if ("quit" == data) {
                break;
            }
        }

        const double seconds = duration_cast<duration<double>>(high_resolution_clock::now() - start).count();

        std::cout << "Time passed: " << seconds << ", Count: " << count << ", bytesRead: " << bytesRead << std::endl;
        std::cout << "Bandwidth: " << (bytesRead / (1024 * 1024 * seconds))  << " Mbps"<< std::endl;
    }

    void Producer()
    {
        SharedData shData = createSharedMapping();

        std::string payload = std::string(1024 * 128, 'x');
        for (uint32_t idx = 0; idx < 1'00'000; ++idx)
        {
            // payload = std::format("Hello_{}", idx + 1);
            SemaphoreGuard guard { shData.semaphoreWriter, shData.semaphoreReader };

            shData.sharedDataBlock->size = payload.length();
            memcpy(shData.sharedDataBlock->buffer.data(), payload.data(), payload.size());
        }

        payload.assign("quit");
        shData.sharedDataBlock->size = payload.length();
        memcpy(shData.sharedDataBlock->buffer.data(), payload.data(), payload.size());
    }

    void MultiprocessTest()
    {
        const pid_t pid = fork();
        if (0 == pid)
            Producer(); /** Child **/
        else if (pid > 0)
            Consumer();  /** Parent **/
    }
};


void SharedMemoryDataExchangeEx::TestAll([[maybe_unused]] const std::vector<std::string_view> &params)
{
    // SharedData sharedData = createSharedMapping();

    // SharedMemoryDataExchangeEx::Test();
    // ExchangeMessages::Producer_Consumer(params);
    // ExchangeMessagesMultiprocess::MultiprocessTest();
    ExchangeMessagesMultiprocess_LoadTest::MultiprocessTest();
}