/**============================================================================
Name        : SharedMemoryChannel.cpp
Created on  : 12.07.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryChannel.cpp
============================================================================**/

#include "SharedMemoryChannel.hpp"
#include "Random.hpp"
#include "DateTimeUtilities.hpp"

#include <iostream>
#include <print>
#include <format>
#include <string>
#include <string_view>
#include <syncstream>
#include <filesystem>
#include <fstream>

#include "semaphore.h"

#include <cstdio> /* popen(), perror() */
#include <cstdint>

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
#include <sys/wait.h> /* wait, sleep */
#include <syslog.h> /* syslog */
#include <termios.h>
#include <thread>
#include <unistd.h> /* read, fork, ftruncate */

#define DEBUG(Stream) std::osyncstream { Stream }  << '[' << getCurrentTime() << "] "
#define LOG  DEBUG(std::cout)
#define ERR  DEBUG(std::cerr)

namespace
{
    using namespace utilities::random;
    using namespace DateTimeUtilities;

    using Handle = int32_t;
    constexpr int32_t InvalidHandle { -1 };

    struct SharedData
    {
        using size_type = size_t;

        static constexpr size_type SemaphoreNameSize { 32 };
        static constexpr size_type BufferSize { 1024 * 1024 };

        // std::atomic<int32_t> useCount { 0 };
        std::array<char, SemaphoreNameSize> semaphoreName {};
        size_type size { 0 };
        std::array<char, BufferSize> buffer {};
    };

    /*
    // template<typename Ty>
    struct Guard
    {
        Guard& semRelease { nullptr };

        SemaphoreGuard(sem_t* semWait, sem_t* semRelease): semRelease { semRelease }
        {
            ::sem_wait(semWait);
        }

        ~SemaphoreGuard() {
            ::sem_post(semRelease);
        }
    };
    */

    struct SharedMemoryChannel
    {
        [[nodiscard]]
        bool openSharedMemorySegment()
        {
            /// Both O_CREAT and O_EXCL were specified to shm_open() and the shared memory object specified by name already exists.
            handle = ::shm_open(sharedSegmentName.data(),O_CREAT | O_RDWR | O_EXCL | O_TRUNC, S_IRWXU | S_IRWXG);

            if (InvalidHandle != handle)
            {
                if (InvalidHandle == ::ftruncate(handle, SharedData::BufferSize)) {
                    ERR << "mmap ftruncate(). Error = " << errno << std::endl;
                    return false;
                }
            }
            else
            {
                /** Shared memory already exist. **/
                if (EEXIST == errno) {
                    handle = ::shm_open(sharedSegmentName.data(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
                }
                else
                { // TODO: Use std::source_location
                    // throw std::runtime_error("shm_open() failed. Error = " + std::to_string(errno));
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]]
        bool createSharedMapping()
        {
            void *area = ::mmap(nullptr,sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, handle, 0);
            if (MAP_FAILED == area) {
                ERR << "mmap failed(). Error = " << errno << std::endl;
                return false;
            }

            sharedData = static_cast<SharedData*>(area);
            // const uint32_t useCount = sharedData.incrementUseCount();
            return semaphoreInit();
        }

        [[nodiscard]]
        bool semaphoreInit()
        {
            const std::string randSemaphoreName = randomString(SharedData::SemaphoreNameSize);
            std::copy_n(randSemaphoreName.data(), SharedData::SemaphoreNameSize, sharedData->semaphoreName.data());

            semaphore = ::sem_open(sharedData->semaphoreName.data(), O_CREAT, 0777, 0);
            if (SEM_FAILED == semaphore) {
                return false;
            }
            if (!createSemNameFile(randSemaphoreName)) {
                ERR << "createSemNameFile() failed\n";
            }
            return true;
        }

        [[nodiscard]]
        bool semaphoreOpen()
        {
            /** Sleep 1 second ??? **/
            semaphore = ::sem_open(sharedData->semaphoreName.data(), O_CREAT );
            if (SEM_FAILED == semaphore) {
                return false;
            }
            return true;
        }

        void semaphoreClose() const
        {
            if (InvalidHandle == ::sem_close(semaphore)) {
                ERR << "sem_close() failed. Error = " << errno << std::endl;
            }
            if (InvalidHandle == ::sem_unlink(sharedData->semaphoreName.data())) {
                ERR << "sem_unlink() failed. Error = " << errno << std::endl;
            }
            if (!removeSemNameFile()) {
                ERR << "removeSemNameFile() failed\n";
            }
        }

        void closeSharedMapping()
        {
            if (InvalidHandle == ::munmap(sharedData, sizeof(SharedData))) {
                ERR << "munmap() failed\n";
            }
            if (InvalidHandle == ::close(handle)) {
                ERR << "close() failed\n";
            }
            if (InvalidHandle == ::shm_unlink(sharedSegmentName.data())) {
                ERR << "shm_unlink() failed\n";
            }
        }

        static bool createSemNameFile(const std::string& name)
        {
            const std::filesystem::path fileName  { semPidFile };
            if (std::ofstream file(fileName); file.good()) {
                file.write(name.data(), name.size()).flush();
                return std::filesystem::file_size(fileName) == name.size();
            }
            return false;
        }

        static bool removeSemNameFile()
        {
            if (const std::filesystem::path fileName  { semPidFile }; std::filesystem::exists(fileName)) {
                return std::filesystem::remove(fileName);
            }
            return false;
        }

        /** FIXME: Delete ??? **/
        [[nodiscard]]
        static bool checkSystemSemFile(const std::string& name)
        {
            const std::filesystem::path semaphorePath { std::filesystem::path { shmFsPath }.concat(name) };
            return std::filesystem::exists(semaphorePath);
        }

    private:
        constexpr static std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_00000__1__" };
        constexpr static std::string_view shmFsPath  { R"(/dev/shm/sem.)" };
        constexpr static std::string_view semPidFile { R"(/tmp/semaphore_name.sem)" };

        Handle handle { InvalidHandle };

        /** FIXME: --> std::unique_ptr<T> **/
        sem_t* semaphore { SEM_FAILED };

        /** FIXME: --> std::unique_ptr<T> **/
        SharedData* sharedData { nullptr };
    };
}


void ipc::shared_memory_channel::TestSharedMemoryChannel()
{
    /*
    SharedMemoryChannel channel;
    channel.semaphoreInit();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    channel.semaphoreClose();
    */
}


/**
1. Server создает Shared Mem

    ::shm_open
        OK     -->  ::ftruncate

                --> Init Semaphore
                    --> Unlink if Exist

                --> Store File WIth name

        False  -->  ::shm_open

            --> Open Semaphore
                --> Unlink Semaphore File
**/