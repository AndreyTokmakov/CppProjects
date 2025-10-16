/**============================================================================
Name        : MultiprocessQueue.cpp
Created on  : 30.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ MultiprocessQueue
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <set>
#include <array>
#include <thread>
#include <atomic>
#include <syncstream>
#include <random>

/* POSIX only headers. */
#include <arpa/inet.h>
// #include <curses.h>
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h> /* creat, O_CREAT */
#include <libgen.h>
#include <monetary.h> /* strfmon */
#include <netdb.h> /* gethostbyname */
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h> /* getpwuid, getpwnam, getpwent */
#include <regex.h>
#include <sched.h>
#include <sys/mman.h> /* mmap, munmap */
#include <sys/ipc.h>
#include <sys/resource.h> /* rusage, getrusage, rlimit, getrlimit */
#include <sys/select.h> /* select, FD_ZERO, FD_SET */
#include <sys/sem.h> /* semget, semop, semctl */
#include <sys/shm.h> /* shmget, shmat, etc. */
#include <sys/socket.h>
#include <sys/stat.h> /* S_IRUSR and family, */
#include <sys/types.h> /* pid_t */
#include <sys/time.h>
#include <sys/utsname.h> /* uname, struct utsname */
#include <sys/wait.h> /* wait, sleep */
#include <syslog.h> /* syslog */
#include <termios.h>
#include <unistd.h> /* read, fork, ftruncate */

#include "MultiprocessQueue.h"


namespace BasicRingBuffer
{
    template<typename T,
            size_t Capacity = 10>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>,
                      "ERROR: Type of the RingBuffer can not be void");
        static_assert(0 != Capacity,
                      "Please try a little bigger buffer");
        // static_assert(is_pow_of_2(Capacity), "Capacity shall be power of 2");

        size_type writePos { 0 };
        size_type readPos { 0 };
        bool overflow { false };

        /** TODO: make heap allocated ?? **/
        std::array<value_type, Capacity> buffer {};

        bool read(value_type& output) noexcept
        {
            if ((overflow && writePos > readPos) || (!overflow && readPos >= writePos))
                return false;

            output = buffer[readPos++];
            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }

            return true;
        }

        bool push(value_type value) noexcept
        {
            buffer[writePos++] = value;
            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            } else if (overflow && writePos >= readPos) {
                ++readPos;
            }
            return true;
        }
    };
}

namespace BasicRingBuffer::Tests
{
    template<typename T,
            size_t Capacity = 10>
    void info(const RingBuffer<T, Capacity>& buffer)
    {
        std::cout << "{ ";
        for (const T& element : buffer.buffer)
            std::cout << element << ' ';
        std::cout << "} [writePos: " << buffer.writePos << ", readPos: "
                  << buffer.readPos << ", overflow: " << std::boolalpha << buffer.overflow << "]" << std::endl;
    }

    template<typename T,
            size_t Capacity = 10>
    void push(const T& val, RingBuffer<T, Capacity>& buffer)
    {
        std::cout << "PUSH: " << val << " ==> " << std::boolalpha << buffer.push(val) << " | ";
        info(buffer);
    }

    template<typename T,
            size_t Capacity = 10>
    void read(RingBuffer<T, Capacity>& buffer)
    {
        T val {};
        if (const bool result = buffer.read(val); result) {
            std::cout << "READ: " << val << " <== "<< std::boolalpha << result << " | ";
        } else {
            std::cout << "READ: " << " <== "<< std::boolalpha << result << " | ";
        }
        info(buffer);
    }

    void SimpleTest()
    {
        RingBuffer<int, 3> buffer {};

        push(1, buffer);
        push(2, buffer);
        push(3, buffer);
        push(4, buffer);

        read(buffer);
        read(buffer);
        read(buffer);
        read(buffer);

        push(5, buffer);
        push(6, buffer);

        read(buffer);
        read(buffer);

        push(7, buffer);
        push(8, buffer);
        push(9, buffer);
        push(10, buffer);
        push(11, buffer);

        read(buffer);
        read(buffer);
        read(buffer);
    }
}


namespace MultiThreaded_LockBased_RingBuffer
{
    template<typename T,
            size_t Capacity = 10>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>,
                      "ERROR: Type of the RingBuffer can not be void");
        static_assert(0 != Capacity,
                      "Please try a little bigger buffer");
        std::mutex mtx;

        size_type writePos { 0 };
        size_type readPos { 0 };
        bool overflow { false };

        /** TODO: make heap allocated ?? **/
        std::array<value_type, Capacity> buffer {};

        bool read(value_type& output) noexcept
        {
            std::lock_guard<std::mutex> lock(mtx);
            if ((overflow && writePos > readPos) || (!overflow && readPos >= writePos))
                return false;

            output = buffer[readPos++];
            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }
            return true;
        }

        bool push(value_type value) noexcept
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer[writePos++] = value;
            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            } else if (overflow && writePos >= readPos) {
                // ++readPos;
                --writePos;
                return false;
            }
            return true;
        }
    };
}


namespace MultiThreaded_LockBased_RingBuffer::Tests
{
    void SimpleTest()
    {
        constexpr size_t valCountToPush { 10'000 };
        using ValueType = int;

        RingBuffer<ValueType, 100> buffer {};
        std::mutex mtxResults;
        std::vector<ValueType> readValues {};
        readValues.reserve(valCountToPush);

        std::atomic_bool done {false};
        std::vector<std::jthread> tasks {};

        for (int i = 0; i < 1; ++i) {
            tasks.emplace_back([&] {
                while (true) {
                    ValueType val { 0 };
                    if (const bool _ = buffer.read(val)) {
                        std::lock_guard<std::mutex> lock(mtxResults);
                        readValues.push_back(val);
                    } else if (done.load(std::memory_order::relaxed)) {
                        break;
                    }
                }
                std::osyncstream {std::cout} << "Consumer: done " << std::endl;
            });
        }

        tasks.emplace_back([&] {
            int val { 0 };
            for (size_t idx = 0UL, n = 0UL; idx < valCountToPush; ++idx, ++n) {
                if (false == buffer.push(++val)) {
                    --idx;
                }
            }
            done.store(true, std::memory_order::relaxed);
            std::osyncstream {std::cout} << "Producer: done " << std::endl;
        });

        for (auto& task : tasks)
            task.join();

        std::osyncstream {std::cout} << "Done: " << readValues.size() << std::endl;
        const std::set<ValueType> unique(readValues.begin(), readValues.end());
        std::cout << unique.size() << std::endl;
    }
}

namespace SharedMemory_LockBased_RingBuffer
{
#define RESULT_OK (0)
#define RESULT_FAILURE (-1)
#define INVALID_HANDLE  (-1)
#define ASSERT_NOT(error_value, actual, func_name) \
    if (error_value == actual)           \
        throw std::runtime_error(std::string(func_name) + "() failed. Error = " + std::to_string(errno));

    std::string randomString(size_t size)
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

    bool logError(const std::string &func) noexcept
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return false;
    }

    enum class Type
    {
        Consumer,
        Producer
    };

    template<typename T,
            size_t Capacity = 10>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>,
                      "ERROR: Type of the RingBuffer can not be void");
        static_assert(0 != Capacity,
                      "Please try a little bigger buffer");

        struct Header
        {
            static inline constexpr uint16_t semaphoreNameSize { 32 };
            uint32_t useCount {0};
            std::array<char, semaphoreNameSize> semaphoreName {};
        };

        struct SharedData
        {
            Header header {};
            std::array<char, 1024> buffer {};
        };

        std::mutex mtx;

        size_type writePos { 0 };
        size_type readPos { 0 };
        bool overflow { false };

        Type queueType { Type::Producer };

        int32_t shmHandle { -1 };
        sem_t* semaphore {};

        std::string sharedSegmentName {};
        std::string semaphoreName {};

        SharedData* sharedData { nullptr };

        /** TODO: make heap allocated ?? **/
        std::array<value_type, Capacity> buffer {};

        explicit RingBuffer(std::string segmentName, const Type qType = Type::Producer):
                queueType { qType }, sharedSegmentName { std::move(segmentName) }
        {
            createSharedMemSegment();
            createMapping();
            initSemaphoreName();
            openSemaphore();
        }

        bool read(value_type& output) noexcept
        {
            std::lock_guard<std::mutex> lock(mtx);
            if ((overflow && writePos > readPos) || (!overflow && readPos >= writePos))
                return false;

            output = buffer[readPos++];
            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }
            return true;
        }

        bool push(value_type value) noexcept
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer[writePos++] = value;
            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            } else if (overflow && writePos >= readPos) {
                // ++readPos;
                --writePos;
                return false;
            }
            return true;
        }

        void createSharedMemSegment()
        {
            // TODO: Producer rename to -> Creator ??
            if (Type::Producer == queueType)
            {
                shmHandle = ::shm_open(sharedSegmentName.data(),
                                       O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
                ASSERT_NOT(INVALID_HANDLE, shmHandle, "shm_open");

                const int retCode = ::ftruncate(shmHandle, sizeof(SharedData));
                ASSERT_NOT(INVALID_HANDLE, retCode, "ftruncate");
                std::cout << "Producer : shared segment created" << std::endl;
            }
            else
            {
                shmHandle= ::shm_open(sharedSegmentName.data(),
                                      O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
                ASSERT_NOT(INVALID_HANDLE, shmHandle, "shm_open");
                std::cout << "Consumer : shared segment opened" << std::endl;
            }
        }

        void createMapping()
        {
            void *area = ::mmap(nullptr,
                                sizeof(Header),
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                shmHandle,
                                0);
            ASSERT_NOT(MAP_FAILED, area, "mmap");

            sharedData = reinterpret_cast<SharedData*>(area);
            ASSERT_NOT(nullptr, sharedData, "reinterpret_cast<Data*>(area)");
        }

        // TODO: Keep counter of consumers/producers: Only first one shall init Semaphore name
        //       - Combine initSemaphoreName() + openSemaphore()
        void initSemaphoreName()
        {
            // TODO: Producer rename to -> Creator ??
            if (Type::Producer == queueType)
            {
                semaphoreName = randomString(Header::semaphoreNameSize);
                memcpy(sharedData->header.semaphoreName.data(),semaphoreName.data(), Header::semaphoreNameSize);
                std::cout << "Producer: Semaphore name is set to '" << semaphoreName << "'" << std::endl;
            }
            else {
                semaphoreName.assign(sharedData->header.semaphoreName.data(), Header::semaphoreNameSize);
                std::cout << "Consumer: Semaphore name = '" << semaphoreName << "'" << std::endl;
            }
        }

        // TODO: Keep counter of consumers/producers
        //       - Last one shall close all. If no 'semaphore' created --> shall be created
        //       - Combine initSemaphoreName() + openSemaphore()
        void openSemaphore()
        {
            // TODO: Producer rename to -> Creator ??
            if (Type::Producer == queueType)
            {
                semaphore = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
                ASSERT_NOT(SEM_FAILED, semaphore, "sem_open");
                std::cout << "Producer: Semaphore '" << semaphoreName << "' created " << std::endl;
            }
            else
            {
                semaphore = ::sem_open(semaphoreName.data(), O_CREAT );
                ASSERT_NOT(SEM_FAILED, semaphore, "sem_open");
                std::cout << "Consumer: Semaphore '" << semaphoreName << "' opened " << std::endl;
            }
        }

        // TODO: Keep counter of consumers/producers | Closing depends of counter
        [[nodiscard]]
        bool closeSemaphore() const noexcept
        {
            std::cout << "Closing semaphore '" << semaphoreName << "'" << std::endl;
            if (RESULT_OK != ::sem_close(semaphore)) {
                return logError("sem_close()");
            }
            std::cout << "Unlink semaphore '" << semaphoreName << "'" << std::endl;
            if (RESULT_OK != ::sem_unlink(semaphoreName.data())) {
                return logError("sem_unlink()");
            }
            return true;
        }

        // TODO: Keep counter of consumers/producers | Closing depends of counter
        [[nodiscard]]
        bool closeSharedMem() const noexcept
        {
            std::cout << "Closing shared memory segment " << shmHandle  << std::endl;
            if (RESULT_OK != ::close(shmHandle)) {
                return logError("close()");
            }

            std::cout << "Unlink shared memory segment " << sharedSegmentName  << std::endl;
            if (RESULT_OK != ::shm_unlink(sharedSegmentName.data())) {
                return logError("shm_unlink()");
            }
            return true;
        }

        // TODO: Keep counter of consumers/producers | Closing depends of counter
        ~RingBuffer()
        {
            if (Type::Producer == queueType)
            {
                std::cout << "Producer : closing" << std::endl;
                [[maybe_unused]] auto res1 = closeSemaphore();
                [[maybe_unused]] auto res2 = closeSharedMem();
            }
            else
            {
                std::cout << "Consumer : closing" << std::endl;
            }
        }
    };
}

namespace SharedMemory_LockBased_RingBuffer::Tests
{
    const std::string sharedMemoryObjName {"__SHARED_MEMORY_ATOMIC_OBJECT__1__" };


    void CreateConsumer()
    {
        std::this_thread::sleep_for(std::chrono::seconds (1));
        RingBuffer<int, 10> buffer { sharedMemoryObjName, Type::Consumer };
    }

    void CreateProducer()
    {
        RingBuffer<int, 10> buffer { sharedMemoryObjName, Type::Producer };
        std::this_thread::sleep_for(std::chrono::seconds (2));
    }

    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            CreateConsumer();
        }
        else if (pid > 0) { /** Parent **/
            CreateProducer();
        }
        else {
            std::cerr << "Unable to create child process" << std::endl;
        }
    }
}


// TODO: Builtin GCC Atomics:
//  https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Atomic-Builtins.html

// TODO: Ring Buffer:
//  https://github.com/bo-yang/ring_buffer/blob/master/ring_buffer.hh

// TODO: https://stackoverflow.com/questions/69817291/how-to-synchronize-multiple-processes-in-c
// TODO:
//  - Make tests SharedMem Exchange VS Unit Sockets


void MultiprocessQueue::TestAll()
{
    // BasicRingBuffer::Tests::SimpleTest();
    // MultiThreaded_LockBased_RingBuffer::Tests::SimpleTest();
    SharedMemory_LockBased_RingBuffer::Tests::MultiProcessTest();
};