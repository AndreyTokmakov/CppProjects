/**============================================================================
Name        : SharedMutex.cpp
Created on  : 03.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMutex.cpp
============================================================================**/

#include "SharedMutex.h"
#include "../common.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <csignal>
#include <utility>

#define RESULT_OK       ( 0)
#define INVALID_HANDLE  (-1)

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

namespace SharedMutex::SharedMemoryUtilities
{
    constexpr std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_00000__3__" };

    int error(const std::string &func) {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    struct SharedDataHeader
    {
        uint32_t useCount { 0 };
    };

    struct SharedDataBlock
    {
        SharedDataHeader header { 0 };
        uint64_t someTestCounter { 0 };
        pthread_mutex_t mtx;
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

namespace SharedMutex::MutexUtilities
{
    struct Mutex
    {
        pthread_mutex_t * mutex { nullptr };

        explicit Mutex(void *shmMemMutex): mutex { (pthread_mutex_t*)shmMemMutex }
        {
            pthread_mutexattr_t attr;
            ::pthread_mutexattr_init(&attr);
            ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            ::pthread_mutexattr_settype(&attr,  PTHREAD_MUTEX_FAST_NP);

            if (RESULT_OK != ::pthread_mutex_init(mutex, &attr))
            {
                ::free(mutex);
                throw std::runtime_error("Unable to create mutex");
            }
            // std::cout << "mutex = " << mutex << std::endl;
            // lock();
        }

        ~Mutex()
        {
            // unlock();
            ::pthread_mutex_destroy(mutex);
        }

        void lock()
        {
            const int32_t result = ::pthread_mutex_lock(mutex);
            if (RESULT_OK != result) {
                // throw std::runtime_error("Unable to lock mutex");
                std::cout << "Unable to lock mutex"<< std::endl;
            }
            else {
                std::cout << "Locked(" << mutex << ")\n";
            };
        }

        void unlock()
        {
            const int32_t result = ::pthread_mutex_unlock(mutex);
            if (RESULT_OK != result) {
                //throw std::runtime_error("Unable to UNlock mutex");
                std::cout << "Unable to UNlock mutex"<< std::endl;
            }
            else {
                // std::cout << "UNLocked. Result = " << result << std::endl;
            };
        }

        bool tryLock()
        {
            if (const int tryResult = ::pthread_mutex_trylock(mutex);
                RESULT_OK != tryResult)
            {
                if (EBUSY == tryResult)
                    return false;
                throw std::runtime_error("Unable to tryLock mutex");
            }
            return true;
        }
    };
}

namespace SharedMutex
{
    using namespace SharedMemoryUtilities;
    using namespace MutexUtilities;

    void info(const SharedData& data)
    {

        std::cout << "Shared segment info:\n";
        std::cout << "\tHeader location: " << &data.handle << std::endl;
        std::cout << "\tBlock  location: " << &data.sharedDataBlock << std::endl;
        std::cout << "\tsomeTestCounter: " << data.sharedDataBlock->someTestCounter << std::endl;
    }

    void ProcParent()
    {
        SharedData data = createSharedMapping();
        ++data.sharedDataBlock->someTestCounter;
        //info(data);

        {
            std::cout << "MAIN  - Entered\n";
            Mutex mtx {&data.sharedDataBlock->mtx};
            mtx.lock();
            // std::cout << "MAIN  - Locked(" << mtx.mutex << ")\n";
            std::this_thread::sleep_for(std::chrono::seconds (5));

            mtx.unlock();
            std::cout << "MAIN  - Unlocked\n";
        }
        std::cout << data.sharedDataBlock->someTestCounter << std::endl;
    }

    void ProcChild()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (100));
        SharedData data = createSharedMapping();
        ++data.sharedDataBlock->someTestCounter;
        //info(data);

        {
            std::cout << "CHILD - Entered\n";
            Mutex mtx {&data.sharedDataBlock->mtx};
            mtx.lock();
            // std::cout << "CHILD - Locked(" << mtx.mutex << ")\n";
            mtx.unlock();
            std::cout << "CHILD - Unlocked\n";
        }

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
    }
};

namespace SharedMutex::Simple
{
   void ProcessOne()
   {
       int32_t handle = ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);
       ASSERT_NOT(INVALID_HANDLE, handle, "shm_open");

       const int retCode = ::ftruncate(handle, sizeof(pthread_mutex_t));
       ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");


       void *area = ::mmap(nullptr,
                           sizeof(pthread_mutex_t),
                           PROT_READ | PROT_WRITE, MAP_SHARED,
                           handle,
                           0);
       ASSERT_NOT(MAP_FAILED, area, "mmap");

       {
           pthread_mutex_t * mutex {static_cast<pthread_mutex_t *>(area)};

           pthread_mutexattr_t attr;
           ::pthread_mutexattr_init(&attr);
           ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
           ::pthread_mutexattr_settype(&attr,  PTHREAD_MUTEX_FAST_NP);

           if (RESULT_OK != ::pthread_mutex_init(mutex, &attr))
           {
               ::free(mutex);
               std::cout << "Unable to create mutex" << std::endl;
           } else
           {
               std::cout << "Mutex(" << mutex << ")\n";
           }

           std::cout << "Waiting\n";
           int32_t result = ::pthread_mutex_lock(mutex);
           if (RESULT_OK != result) {
               // throw std::runtime_error("Unable to lock mutex");
               std::cout << "Unable to lock mutex"<< std::endl;
           }
           else {
               std::cout << "Locked(" << mutex << ")\n";
           };

           std::this_thread::sleep_for(std::chrono::seconds (5));

           result = ::pthread_mutex_unlock(mutex);
           if (RESULT_OK != result) {
               std::cout << "Unable to UNlock mutex"<< std::endl;
           } else {
               std::cout << "Unlocking Mutex(" << mutex << ")\n";
           }

           ::pthread_mutex_destroy(mutex);
       }

       std::this_thread::sleep_for(std::chrono::seconds (5));

       int result = munmap(area, sizeof(pthread_mutex_t));
       ASSERT_NOT(INVALID_HANDLE, result, "munmap");

       std::cout << "Closing shared memory [handle: " << handle << ", name: " << sharedSegmentName << "]\n";
       if (RESULT_OK != ::close(handle)) {
           error("close()");
       }

       std::cout << "Unlink shared memory segment " << sharedSegmentName << std::endl;
       if (RESULT_OK != ::shm_unlink(sharedSegmentName.data())) {
           error("shm_unlink()");
       }
   }

   void ProcessTwo()
   {

       int32_t handle = ::shm_open(sharedSegmentName.data(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
       ASSERT_NOT(INVALID_HANDLE, handle, "shm_open");

       const int retCode = ::ftruncate(handle, sizeof(pthread_mutex_t));
       ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");


       void *area = ::mmap(nullptr,
                           sizeof(pthread_mutex_t),
                           PROT_READ | PROT_WRITE, MAP_SHARED,
                           handle,
                           0);
       ASSERT_NOT(MAP_FAILED, area, "mmap");

       {
           pthread_mutex_t * mutex {static_cast<pthread_mutex_t *>(area)};

           pthread_mutexattr_t attr;
           ::pthread_mutexattr_init(&attr);
           ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
           ::pthread_mutexattr_settype(&attr,  PTHREAD_MUTEX_FAST_NP);

           if (RESULT_OK != ::pthread_mutex_init(mutex, &attr))
           {
               ::free(mutex);
               std::cout << "Unable to create mutex" << std::endl;
           } else {
               std::cout << "Mutex(" << mutex << ")\n";
           }

           std::cout << "Waiting\n";
           int32_t result = ::pthread_mutex_lock(mutex);
           if (RESULT_OK != result) {
               std::cout << "Unable to lock mutex"<< std::endl;
           } else {
               std::cout << "Locked(" << mutex << ")\n";
           };

           result = ::pthread_mutex_unlock(mutex);
           if (RESULT_OK != result) {
               std::cout << "Unable to UNlock mutex"<< std::endl;
           } else {
               std::cout << "Unlocking Mutex(" << mutex << ")\n";
           }
       }
   }
}

void SharedMutex::TestAll()
{
    // MultiProcessTest();
    // ProcParent();
    // ProcChild();

    // Simple::ProcessOne();
    Simple::ProcessTwo();

}