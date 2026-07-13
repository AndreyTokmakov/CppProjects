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
        static constexpr size_type BufferSize { 32UL * 1024 };

        std::array<char, SemaphoreNameSize> semaphoreName {};
        size_type size { 0 };
        std::array<char, BufferSize> buffer {};
    };

    struct SemaphoreGuard
    {
        SemaphoreGuard(sem_t* sem2Wait, sem_t* sem2Release): semRelease { sem2Release } {
            ::sem_wait(sem2Wait);
        }

        ~SemaphoreGuard() {
            ::sem_post(semRelease);
        }

        SemaphoreGuard(const SemaphoreGuard&) = delete;
        SemaphoreGuard& operator=(const SemaphoreGuard&) = delete;

        SemaphoreGuard(SemaphoreGuard&&) noexcept = delete;
        SemaphoreGuard& operator=(SemaphoreGuard&&) noexcept = delete;

    private:
        sem_t* semRelease { nullptr };
    };

    struct SharedMemoryChannel
    {
        ~SharedMemoryChannel()
        {
            sharedData.reset();

            // Move to functions
            if (InvalidHandle != handle)
            {
                if (0 != ::close(handle)) {
                    ERR << "close(). Error = " << errno << std::endl;
                }
                else {
                    LOG << "close() OK. Handle = " << handle << std::endl;
                }
            }
        }

        bool InitializeOwner()
        {
            if (!createSharedMemorySegment()) {
                return false;
            }
            if (!createMapping()) {
                return false;
            }
            return true;
        }

        bool InitializeClient()
        {
            return true;
        }

        [[nodiscard]]
        bool createSharedMemorySegment()
        {
            if (InvalidHandle != handle) {
                ERR << "SharedMemory handle already initialized" << std::endl;
                return false;
            }

            /// Both O_CREAT and O_EXCL were specified to shm_open() and the shared memory object specified by name already exists.
            handle = ::shm_open(sharedSegmentName.data(),
                                O_CREAT | O_RDWR | O_EXCL | O_TRUNC,
                                S_IRWXU | S_IRWXG);
            if (InvalidHandle == handle) {
                ERR << "shm_open(). Error = " << errno << std::endl;
                return false;
            }
            LOG << "Shared Memory segment created. Name: " << sharedSegmentName << ", Handle: " << handle << std::endl;

            if (InvalidHandle == ::ftruncate(handle, sizeof(SharedData))) {
                ERR << "ftruncate(). Error = " << errno << std::endl;
                return false;
            }

            LOG << "ftruncate() OK. Name: " << sharedSegmentName  << ", Size: " << sizeof(SharedData) << std::endl;
            return true;
        }

        [[nodiscard]]
        bool openSharedMemorySegment()
        {
            if (InvalidHandle != handle) {
                ERR << "SharedMemory handle already initialized" << std::endl;
                return false;
            }

            handle = ::shm_open(sharedSegmentName.data(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
            if (InvalidHandle == handle) {
                ERR << "shm_open(). Error = " << errno << std::endl;
                return false;
            }

            LOG << "Shared Memory segment opened. Name: " << sharedSegmentName << std::endl;
            return true;
        }

        [[nodiscard]]
        bool createMapping()
        {
            void *area = ::mmap(nullptr,
                                sizeof(SharedData),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                handle,
                                0);
            if (MAP_FAILED == area) {
                ERR << "mmap failed(). Error = " << errno << std::endl;
                return false;
            }

            LOG << "mmap() OK. Ptr = " << area << std::endl;

            sharedData = SharedDataPtr(static_cast<SharedData*>(area), closeMapping);
            // sharedData = static_cast<SharedData*>(area);

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


        void unlinkMapping()
        {
            if (InvalidHandle == ::shm_unlink(sharedSegmentName.data())) {
                ERR << "shm_unlink() failed\n";
            }
            else {
                LOG << "shm_unlink(" << sharedSegmentName.data() << ") OK\n";
            }
        }

        /** TODO: ==> Make struct **/
        static bool closeMapping(SharedData* ptr)
        {
            if (ptr == nullptr) {
                LOG << "munmap(" << ptr << ") Skipp\n";
                return true;
            }
            if (InvalidHandle == ::munmap(ptr, sizeof(SharedData))) {
                ERR << "munmap(" << ptr << ") failed. Error = " << errno << std::endl;
                return false;
            }

            LOG << "munmap(" << ptr << ") OK\n";
            return true;
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

        using SharedDataPtr = std::unique_ptr<SharedData, decltype(&SharedMemoryChannel::closeMapping)>;
        SharedDataPtr sharedData { SharedDataPtr (nullptr, closeMapping )};
    };
}


namespace tests
{
#define DGB_PARENT LOG << "[Parent] "
#define DGB_CHILD  LOG << "[Child ] "

    void createAndCloseSharedMemory()
    {
        SharedMemoryChannel channel;
        channel.InitializeOwner();

        std::this_thread::sleep_for(std::chrono::seconds(3));

        channel.unlinkMapping();
        //channel.closeMapping(channel.sharedData);
    }



    void Parent()
    {
        DGB_PARENT << "Started\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));

        DGB_PARENT << "Completed\n";
    }

    void Child()
    {
        DGB_CHILD << "Started\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        DGB_CHILD << "Completed\n";
    }

    void multiProcessTests()
    {
        if (const pid_t pid = fork(); pid == 0)
            Child();
        else if (pid > 0)
            Parent();
        LOG << "Done" << std::endl;
    }
}

void ipc::shared_memory_channel::TestSharedMemoryChannel()
{
    tests::createAndCloseSharedMemory();
    // tests::multiProcessTests();
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