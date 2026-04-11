/**============================================================================
Name        : SharedMemoryDataExchange.cpp
Created on  : 30.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchange.cpp
============================================================================**/

#include "SharedMemoryDataExchange.h"

#include "SharedMemory.h"
#include "../common.h"
#include "semaphore.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <array>
#include <csignal>


#define INVALID_HANDLE  (-1)


namespace SharedMemoryDataExchange::DemoOne
{
    constexpr std::string_view sharedMemoryObjName {"__SHARED_MEMORY_OBJECT_00000002"};
    constexpr std::string_view semaphoreName { "my_named_semaphore_test" };

    int sharedHandle = -1;

    struct Data
    {
        uint32_t size{0};
        std::array<char, 1024> buffer{};
    };

    void error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
    }

    void closeSemaphore(sem_t *sem)
    {
        if (sem_close(sem) < 0) {
            std::cout << "sem_close() failed. Error = " << errno << std::endl;
        } else {
            std::cout << "Semaphore closed" << std::endl;
        }

        if (sem_unlink(semaphoreName.data()) < 0) {
            std::cout << "sem_unlink() failed. Error = " << errno << std::endl;
        } else {
            std::cout << "Semaphore unlinked" << std::endl;
        }
    }

    void closeSharedMem()
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

    void signalHandler(int /* sigId */)
    {
        closeSharedMem();
        std::exit(0);
    };

    void Consumer()
    {
        sharedHandle = ::shm_open(sharedMemoryObjName.data(),
                                  O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
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

        sem_t *sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
        if (SEM_FAILED == sem) {
            error("sem_open()");
            signalHandler(0); // FIXME
            return;
        }

        // timespec timeout {10, 0};
        while (true)
        {
            std::cout << "Waiting for semaphore...." << std::endl;

            //  const int result = sem_timedwait(sem, &timeout);

            [[maybe_unused]]
            int result = sem_wait(sem);

            std::cout << "Released" << std::endl;

            std::cout << "\tsize  : " << dataPtr->size << std::endl;
            std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;
        }
    }

    void Producer()
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

        dataPtr->size = 16;
        memcpy(dataPtr->buffer.data(), "1111111111111111111111111111", dataPtr->size);

        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            error("sem_open");
        }

        sem_post(sem);
        std::cout << "Semaphore is released." << std::endl;
    }
}


namespace SharedMemoryDataExchange::DemoTwo_Debug
{
    enum class TypeExchange
    {
        Consumer,
        Producer
    };

    template<TypeExchange typeExchange>
    struct Exchange
    {
        struct Data
        {
            uint32_t size {0};
            std::array<char, 32> semaphoreName {};
            std::array<char, 1024> buffer {};
        };

        std::string sharedSegmentName {};

        // TODO: std::string_view  ???
        std::string semaphoreName {};

        int32_t shmHandle {-1};
        Data* dataPtr { nullptr };
        sem_t *sem {};

        explicit Exchange(std::string segmentName): sharedSegmentName { std::move(segmentName) }
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                shmHandle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
                if (INVALID_HANDLE == shmHandle) {
                    error("shm_open()");
                } else { // TODO: Remove
                    std::cout << sharedSegmentName << " segment is opened. Descriptor = " << shmHandle << std::endl;
                }

                const int retCode = ftruncate(shmHandle, sizeof(Data));
                if (INVALID_HANDLE == retCode) {
                    error("ftruncate");
                } else { // TODO: Remove
                    std::cout << "Segment '" << shmHandle << "' has been truncated to " << sizeof(Data) << " bytes\n";
                }
            }
            else
            {
                shmHandle= ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
                if (INVALID_HANDLE == shmHandle) {
                    error("shm_open()");
                } else { // TODO: Remove
                    std::cout << sharedSegmentName << " segment is opened. Descriptor = " << shmHandle << std::endl;
                }
            }


            void *area = ::mmap(nullptr,
                                sizeof(Data),
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                shmHandle,
                                0);
            if (MAP_FAILED == area) {
                error("mmap()");
            }

            dataPtr = reinterpret_cast<Data*>(area);
            if (!dataPtr) {
                std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
            }



            // TODO: To separate func
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                // TODO: semaphoreName <--- generate 32 bytes
                semaphoreName.assign("1234512345_12345_123451234512345");
                memcpy(dataPtr->semaphoreName.data(), semaphoreName.data(), 32);
            }
            else {
                semaphoreName.assign(dataPtr->semaphoreName.data(), 32);
            }

            std::cout << "semaphoreName: " << semaphoreName << std::endl;


            // TODO: To separate func
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
                if (SEM_FAILED == sem) {
                    error("sem_open()");
                    closeSharedMem(); // FIXME
                } else { // FIXME: remove
                    std::cout << "Consumer: " << semaphoreName << " semaphore created\n";
                }
            }
            else
            {
                sem = sem_open(semaphoreName.data(), O_CREAT );
                if (SEM_FAILED == sem) {
                    error("sem_open");
                } else { // FIXME: remove
                    std::cout << "Producer: " << semaphoreName << " semaphore opened\n";
                }
            }
        }

        ~Exchange()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                closeSemaphore();
                closeSharedMem();
            }
        }

        int error(const std::string &func) const
        {
            std::cerr << func << " failed. Error = " << errno << std::endl;
            return errno;
        }

        void closeSemaphore() const noexcept
        {
            if (0 != sem_close(sem)) {
                error("sem_close()");
            }
            if (0 != sem_unlink(semaphoreName.data())) {
                error("sem_unlink()");
            }
        }

        void closeSharedMem() const noexcept
        {
            if (0 != close(shmHandle)) {
                error("close()");
            } else { // TODO: Remove
                std::cout << shmHandle << " handle is closed" << std::endl;
            }

            if (0 != shm_unlink(sharedSegmentName.data())) {
                error("shm_unlink()");
            } else { // TODO: Remove
                std::cout << sharedSegmentName << " segment is removed" << std::endl;
            }
        }

        void ReadMessages()
        {
            // timespec timeout {10, 0};
            while (true)
            {
                std::cout << "Waiting for semaphore...." << std::endl;

                //  const int result = sem_timedwait(sem, &timeout);
                int result = sem_wait(sem);

                std::cout << "Released" << std::endl;
                std::cout << "\tsize  : " << dataPtr->size << std::endl;
                std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;
            }
        }

        void PutMessage(const std::string& message)
        {
            dataPtr->size = message.length();
            memcpy(dataPtr->buffer.data(), message.data(), dataPtr->size);

            sem_post(sem);
            std::cout << "Semaphore is released." << std::endl;
        }
    };


    void test()
    {
        // Exchange<TypeExchange::Consumer> consumer {"__SHARED_MEMORY_OBJECT_00000002"};
        // consumer.ReadMessages();


        Exchange<TypeExchange::Producer> producer {"__SHARED_MEMORY_OBJECT_00000002"};
        for (int i = 0; i < 100; ++i) {
            producer.PutMessage("TestMessage__" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds (1));
        }
    }
}



namespace SharedMemoryDataExchange::DemoTwo
{
    enum class TypeExchange
    {
        Consumer,
        Producer
    };

    template<TypeExchange typeExchange>
    struct Exchange
    {
        struct Data
        {
            uint32_t size {0};
            std::array<char, 32> semaphoreName {};
            std::array<char, 1024> buffer {};
        };

        std::string sharedSegmentName {};

        // TODO: std::string_view  ???
        std::string semaphoreName {};

        int32_t shmHandle {-1};
        Data* dataPtr { nullptr };
        sem_t *sem {};

        explicit Exchange(std::string segmentName): sharedSegmentName { std::move(segmentName) }
        {
            createSharedMemSegment();
            createDataMapping();
            initSemaphoreName();
            openSemaphore();
        }

        bool createSharedMemSegment()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                shmHandle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
                if (INVALID_HANDLE == shmHandle) {
                    return error("shm_open()");
                } else { // TODO: Remove
                    std::cout << sharedSegmentName << " segment is opened. Descriptor = " << shmHandle << std::endl;
                }

                const int retCode = ftruncate(shmHandle, sizeof(Data));
                if (INVALID_HANDLE == retCode) {
                    return error("ftruncate");
                } else { // TODO: Remove
                    std::cout << "Segment '" << shmHandle << "' has been truncated to " << sizeof(Data) << " bytes\n";
                }
            }
            else
            {
                shmHandle= ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
                if (INVALID_HANDLE == shmHandle) {
                    return error("shm_open()");
                } else { // TODO: Remove
                    std::cout << sharedSegmentName << " segment is opened. Descriptor = " << shmHandle << std::endl;
                }
            }

            return true;
        }

        bool createDataMapping()
        {
            void *area = ::mmap(nullptr,
                                sizeof(Data),
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                shmHandle,
                                0);
            if (MAP_FAILED == area) {
                return error("mmap()");
            }

            dataPtr = reinterpret_cast<Data*>(area);
            if (!dataPtr) {
                std::cerr << "Failed to cast " << area << " to the Data pointer" << std::endl;
                return false;
            }

            return true;
        }

        void initSemaphoreName()
        {
            // TODO: To separate func
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                // TODO: semaphoreName <--- generate 32 bytes
                semaphoreName.assign("1234512345_12345_123451234512345");
                memcpy(dataPtr->semaphoreName.data(), semaphoreName.data(), 32);
            }
            else {
                semaphoreName.assign(dataPtr->semaphoreName.data(), 32);
            }

            std::cout << "semaphoreName: " << semaphoreName << std::endl;
        }

        bool openSemaphore()
        {
            // TODO: To separate func
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
                if (SEM_FAILED == sem) {
                    closeSharedMem(); // FIXME
                    return error("sem_open()");
                } else { // FIXME: remove
                    std::cout << "Consumer: " << semaphoreName << " semaphore created\n";
                }
            }
            else
            {
                sem = sem_open(semaphoreName.data(), O_CREAT );
                if (SEM_FAILED == sem) {
                    return error("sem_open");
                } else { // FIXME: remove
                    std::cout << "Producer: " << semaphoreName << " semaphore opened\n";
                }
            }
            return true;
        }

        ~Exchange()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                closeSemaphore();
                closeSharedMem();
            }
        }

        bool error(const std::string &func) const noexcept
        {
            std::cerr << func << " failed. Error = " << errno << std::endl;
            return false;
        }

        bool closeSemaphore() const noexcept
        {
            if (0 != sem_close(sem)) {
                return error("sem_close()");
            }
            if (0 != sem_unlink(semaphoreName.data())) {
                return error("sem_unlink()");
            }
            return true;
        }

        bool closeSharedMem() const noexcept
        {
            if (0 != close(shmHandle)) {
                return error("close()");
            } else { // TODO: Remove
                std::cout << shmHandle << " handle is closed" << std::endl;
            }

            if (0 != shm_unlink(sharedSegmentName.data())) {
                return error("shm_unlink()");
            } else { // TODO: Remove
                std::cout << sharedSegmentName << " segment is removed" << std::endl;
            }
            return true;
        }

        void ReadMessages()
        {
            // timespec timeout {10, 0};
            while (true)
            {
                //  const int result = sem_timedwait(sem, &timeout);
                int result = sem_wait(sem);

                std::cout << "Data" << std::endl;
                std::cout << "\tsize  : " << dataPtr->size << std::endl;
                std::cout << "\tbuffer: " << std::string_view(dataPtr->buffer.data(), dataPtr->size)<< std::endl;
            }
        }

        void PutMessage(const std::string& message)
        {
            dataPtr->size = message.length();
            memcpy(dataPtr->buffer.data(), message.data(), dataPtr->size);
            sem_post(sem);
        }
    };


    void test()
    {
#if 0
        Exchange<TypeExchange::Consumer> consumer {"__SHARED_MEMORY_OBJECT_00000002"};
        consumer.ReadMessages();
#endif

#if 1
        Exchange<TypeExchange::Producer> producer {"__SHARED_MEMORY_OBJECT_00000002"};
        for (int i = 0; i < 100; ++i) {
            producer.PutMessage("TestMessage__" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds (1));
        }
#endif

    }
}


namespace SharedMemoryDataExchange::DemoThree
{

#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

    enum class TypeExchange
    {
        Consumer,
        Producer
    };

    template<TypeExchange typeExchange>
    struct Exchange
    {
        struct Data
        {
            uint32_t size {0};
            std::array<char, 32> semaphoreName {};
            std::array<char, 1024> buffer {};
        };

        static inline constexpr uint16_t semaphoreNameSize { 32 };

        std::string sharedSegmentName {};

        // TODO: std::string_view  ???
        std::string semaphoreName {};

        int32_t shmHandle {-1};
        Data* dataPtr { nullptr };
        sem_t *sem {};

        explicit Exchange(std::string segmentName): sharedSegmentName { std::move(segmentName) }
        {
            createSharedMemSegment();
            createDataMapping();
            initSemaphoreName();
            openSemaphore();
        }

        void createSharedMemSegment()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                shmHandle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
                ASSERT_NOT(INVALID_HANDLE, shmHandle, "shm_open");

                const int retCode = ftruncate(shmHandle, sizeof(Data));
                ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");
            }
            else
            {
                shmHandle= ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
                ASSERT_NOT(INVALID_HANDLE, shmHandle, "shm_open");
            }
        }

        void createDataMapping()
        {
            void *area = ::mmap(nullptr,
                                sizeof(Data),
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                shmHandle,
                                0);
            ASSERT_NOT(MAP_FAILED, area, "mmap");

            dataPtr = reinterpret_cast<Data*>(area);
            ASSERT_NOT(nullptr, dataPtr, "reinterpret_cast<Data*>(area)");
        }

        void initSemaphoreName()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {   // TODO: semaphoreName <--- generate 32 bytes
                semaphoreName.assign("1234512345_12345_123451234512345");
                memcpy(dataPtr->semaphoreName.data(), semaphoreName.data(), semaphoreNameSize);
            }
            else {
                semaphoreName.assign(dataPtr->semaphoreName.data(), semaphoreNameSize);
            }
        }

        void openSemaphore()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
                ASSERT_NOT(SEM_FAILED, sem, "sem_open");
            }
            else
            {
                sem = ::sem_open(semaphoreName.data(), O_CREAT );
                ASSERT_NOT(SEM_FAILED, sem, "sem_open");
            }
        }

        ~Exchange()
        {
            if constexpr (typeExchange == TypeExchange::Consumer)
            {
                closeSemaphore();
                closeSharedMem();
            }
        }

        bool error(const std::string &func) const noexcept
        {
            std::cerr << func << " failed. Error = " << errno << std::endl;
            return false;
        }

        void closeSemaphore() const noexcept
        {
            if (0 != ::sem_close(sem)) {
                error("sem_close()");
            }
            if (0 != ::sem_unlink(semaphoreName.data())) {
                error("sem_unlink()");
            }
        }

        void closeSharedMem() const noexcept
        {
            if (0 != ::close(shmHandle)) {
                error("close()");
            }

            if (0 != ::shm_unlink(sharedSegmentName.data())) {
                error("shm_unlink()");
            }
        }

        void ReadMessages(uint32_t msgCount)
        {
            uint32_t counter = 0;
            uint64_t bytesRead = 0;

            while (msgCount > counter)
            {
                sem_wait(sem);
                std::string_view data(dataPtr->buffer.data(), dataPtr->size);

                bytesRead += dataPtr->size;
                ++counter;
            }

            std::cout << "messages: " << counter << std::endl;
            std::cout << "bytes   : " << bytesRead << std::endl;
        }

        void PutMessage(const std::string& message)
        {
            dataPtr->size = message.length();
            memcpy(dataPtr->buffer.data(), message.data(), dataPtr->size);
            sem_post(sem);
        }
    };


    void test()
    {
        constexpr uint32_t num = 10;

#if 0
        Exchange<TypeExchange::Consumer> consumer {"__SHARED_MEMORY_OBJECT_00000002"};
        consumer.ReadMessages(num);
#endif

#if 1
        std::string message {"111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
                             "33333333333333333333333333333333333333333333333333333333333333333333333333333333334"
                             "555555555555555555555555555555555555555555555555555555555555555555555555555555555555556"
                             "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
                             "777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
                             "8888888888888888888888888888888888888888888888888888888888888888888888888888888"};
        Exchange<TypeExchange::Producer> producer {"__SHARED_MEMORY_OBJECT_00000002"};
        for (uint32_t i = 0; i < num; ++i)
        {
            // producer.PutMessage("TestMessage__" + std::to_string(i));
            producer.PutMessage(message);
        }
#endif

    }
}





void SharedMemoryDataExchange::TestAll([[maybe_unused]] const std::vector<std::string_view> &params)
{
    // DemoOne::Consumer();
    // DemoOne::Producer();

    // DemoTwo_Debug::test();

    // DemoTwo::test();

    DemoThree::test();
}