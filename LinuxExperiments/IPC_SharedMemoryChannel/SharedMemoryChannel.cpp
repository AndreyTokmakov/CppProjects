/**============================================================================
Name        : SharedMemoryChannel.cpp
Created on  : 12.07.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryChannel.cpp
============================================================================**/

#include "SharedMemoryChannel.hpp"

#include <algorithm>

#include "Random.hpp"
#include "FileUtilities.hpp"
#include "DateTimeUtilities.hpp"

#include <iostream>
#include <print>
#include <format>
#include <string>
#include <string_view>
#include <syncstream>
#include <filesystem>
#include <thread>
#include <fstream>

#include <cstdint>
#include <semaphore.h>
#include <fcntl.h> /* creat, O_CREAT */
#include <sys/mman.h> /* mmap, munmap */
#include <sys/stat.h> /* S_IRUSR and family, */
#include <sys/types.h> /* pid_t */
#include <unistd.h> /* read, fork, ftruncate */

namespace
{
    using namespace std::string_view_literals;

    constexpr std::string_view cyan  = "\033[1;36m"sv;
    constexpr std::string_view red   = "\033[1;31m"sv;
    constexpr std::string_view green = "\033[1;32m"sv;
    constexpr std::string_view reset = "\033[0m"sv;

    #define PRINT(Stream) std::osyncstream { Stream }  << '[' << getCurrentTime() << "] "
    #define WITH_COLOR(color, text)  color << text << reset

    #define LOG    PRINT(std::cout) << WITH_COLOR(green, " [DEBUG] ")
    #define ERR    PRINT(std::cerr) << WITH_COLOR(red, " [ERROR] ")
    #define TRACE  PRINT(std::cout) << WITH_COLOR(cyan, " [TRACE] ")
}


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

        std::array<char, SemaphoreNameSize> writeSemaphoreName {};
        std::array<char, SemaphoreNameSize> readSemaphoreName {};
        size_type size { 0 };
        std::array<char, BufferSize> buffer {};
    };

    // TODO: How add timeouts ??
    struct SemaphoreGuard
    {
        SemaphoreGuard(sem_t* sem2Wait, sem_t* sem2Release): semRelease { sem2Release } {
            sem_wait(sem2Wait);
        }

        ~SemaphoreGuard() {
            sem_post(semRelease);
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
                if (0 != close(handle)) {
                    ERR << "close(). Error = " << errno << std::endl;
                }
            }
        }

        bool InitializeOwner()
        {
            unlinkSemaphores();
            if (!createSharedMemorySegment()) {
                return false;
            }
            if (!createMapping()) {
                return false;
            }
            if (!createWriteSemaphore()) {
                return false;
            }
            if (!createReadSemaphore()) {
                return false;
            }
            return true;
        }

        bool InitializeClient()
        {
            if (!openSharedMemorySegment()) {
                return false;
            }
            if (!createMapping()) {
                return false;
            }
            if (!openWriteSemaphore()) {
                return false;
            }
            if (!openReadSemaphore()) {
                return false;
            }
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
            handle = shm_open(sharedSegmentName.data(),
                                O_CREAT | O_RDWR | O_EXCL | O_TRUNC,
                                S_IRWXU | S_IRWXG);
            if (InvalidHandle == handle) {
                ERR << "shm_open(). Error = " << errno << std::endl;
                return false;
            }

            if (InvalidHandle == ftruncate(handle, sizeof(SharedData))) {
                ERR << "ftruncate(). Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        [[nodiscard]]
        bool openSharedMemorySegment()
        {
            if (InvalidHandle != handle) {
                ERR << "SharedMemory handle already initialized" << std::endl;
                return false;
            }

            handle = shm_open(sharedSegmentName.data(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
            if (InvalidHandle == handle) {
                ERR << "shm_open(). Error = " << errno << std::endl;
                return false;
            }

            return true;
        }

        [[nodiscard]]
        bool createMapping()
        {
            void *area = mmap(nullptr,
                                sizeof(SharedData),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                handle,
                                0);
            if (MAP_FAILED == area) {
                ERR << "mmap failed(). Error = " << errno << std::endl;
                return false;
            }

            sharedData = SharedDataPtr(static_cast<SharedData*>(area), closeMapping);
            return true;
        }

        [[nodiscard]]
        bool createWriteSemaphore()
        {
            writeSemaphoreName = randomString(SharedData::SemaphoreNameSize);
            std::copy_n(writeSemaphoreName.data(), SharedData::SemaphoreNameSize, sharedData->writeSemaphoreName.data());
            return initSemaphore(writeReadySemaphore, writeSemaphoreName);
        }

        [[nodiscard]]
        bool createReadSemaphore()
        {
            readSemaphoreName = randomString(SharedData::SemaphoreNameSize);
            std::copy_n(readSemaphoreName.data(), SharedData::SemaphoreNameSize, sharedData->readSemaphoreName.data());
            return initSemaphore(readReadySemaphore, readSemaphoreName);
        }

        [[nodiscard]]
        bool openWriteSemaphore()
        {
            writeSemaphoreName.assign(sharedData->writeSemaphoreName.data(), sharedData->writeSemaphoreName.size());
            return openSemaphore(writeReadySemaphore, writeSemaphoreName);
        }

        [[nodiscard]]
        bool openReadSemaphore()
        {
            readSemaphoreName.assign(sharedData->readSemaphoreName.data(), sharedData->readSemaphoreName.size());
            return openSemaphore(readReadySemaphore, readSemaphoreName);
        }

        void closeWriteSemaphore()
        {
            if (!closeSemaphore(writeReadySemaphore)){
                ERR << "Failed to close WRITE semaphore\n";
            }
            if (!unlinkSemaphore(writeSemaphoreName)){
                ERR << "Failed to unlink WRITE semaphore\n";
            }
        }

        void closeReadSemaphore()
        {
            if (!closeSemaphore(readReadySemaphore)){
                ERR << "Failed to close READ semaphore\n";
            }
            if (!unlinkSemaphore(readSemaphoreName)){
                ERR << "Failed to unlink READ semaphore\n";
            }
        }

        static void unlinkMapping()
        {
            // TODO: Check if called by owner ???
            if (InvalidHandle == shm_unlink(sharedSegmentName.data())) {
                ERR << "shm_unlink() failed\n";
            }
        }

        void write()
        {
            TRACE << "Waiting to be able to Write." << std::endl;
            SemaphoreGuard guard { writeReadySemaphore, readReadySemaphore };
            TRACE << "Writing ......" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            TRACE << "Writing done\n";
        }

        void read()
        {
            TRACE << "Waiting to be able to Read." << std::endl;
            SemaphoreGuard guard { readReadySemaphore, writeReadySemaphore };
            TRACE << "Reading ......" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            TRACE << "Reading done\n";
        }

        void setWriteReady() const
        {
            sem_post(writeReadySemaphore);
        }

        [[nodiscard]]
        bool isWritable() const noexcept {
            return canAcquireSemaphore(writeReadySemaphore);
        }

        [[nodiscard]]
        bool isReadable() const noexcept {
            return canAcquireSemaphore(readReadySemaphore);
        }

    private:

        /** TODO: ==> Make struct **/
        static bool closeMapping(SharedData* ptr)
        {
            if (ptr == nullptr) {
                return true;
            }
            if (InvalidHandle == munmap(ptr, sizeof(SharedData))) {
                ERR << "munmap(" << ptr << ") failed. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        [[nodiscard]]
        static bool canAcquireSemaphore(sem_t* const semaphore) noexcept
        {
            if (int val { 0 }; 0 == sem_getvalue(semaphore, &val)) {
                return val > 0;
            }
            return false;
        }

        [[nodiscard]]
        static bool initSemaphore(sem_t*& sem, const std::string& name)
        {
            if (SEM_FAILED != sem) {
                ERR << "initSemaphore() failed. Already initialized\n";
                return false;
            }

            /** The semaphore is initially blocked **/
            sem = sem_open(name.data(), O_CREAT, 0777, 0);
            if (SEM_FAILED == sem) {
                ERR << "sem_open(" << name << ", 0777, 0) OK\n";
                return false;
            }

            storeSemaphoreName(name);
            return SEM_FAILED != sem;
        }

        [[nodiscard]]
        static bool openSemaphore(sem_t*& sem, const std::string& name)
        {
            if (SEM_FAILED != sem) {
                ERR << "openSemaphore() failed. Already initialized\n";
                return false;
            }

            sem = sem_open(name.data(), O_CREAT);
            if (SEM_FAILED == sem) {
                ERR << "sem_open(" << name << ", O_CREAT) failed. Error = " << errno << "\n";
                return false;
            }

            return SEM_FAILED != sem;
        }

        [[nodiscard]]
        static bool unlinkSemaphore(const std::string& name)
        {
            if (!checkSystemSemFile(name)) {
                return true;
            }
            if (InvalidHandle == sem_unlink(name.data())) {
                ERR << "sem_unlink(" << name << ") failed. Error = " << errno << std::endl;
                return false;
            }

            return true;
        }

        [[nodiscard]]
        static bool closeSemaphore(sem_t*& sem)
        {
            if (InvalidHandle == sem_close(sem)) {
                ERR << "sem_close(" << sem << ") failed. Error = " << errno << std::endl;
                return false;
            }
            sem = SEM_FAILED;
            return true;
        }

        [[nodiscard]]
        static bool storeSemaphoreName(std::string name) {
            return append2File(semFilePath, name.append(1, '\n'));
        }

        static bool unlinkSemaphores()
        {
            std::ranges::for_each(readFileAsLines(semFilePath), [](const std::string& name) {
                const auto _ = unlinkSemaphore(name);
            });
            return truncateFile(semFilePath);
        }

        [[nodiscard]]
        static std::vector<std::string> readFileAsLines(const std::filesystem::path &filePath)
        {
            std::vector<std::string> lines;
            if (std::ifstream file(filePath); file.is_open() && file.good()) {
                while (std::getline(file, lines.emplace_back())) {/** **/ };
                lines.pop_back();
            }
            return lines;
        }

        [[nodiscard]]
        static bool append2File(const std::filesystem::path& filePath,
                                const std::string& text)
        {
            if (std::ofstream file(filePath, std::ios_base::app); file.is_open() && file.good())
            {
                const int32_t pos = file.tellp();
                file.write(text.data(), std::ssize(text));
                return std::ssize(text) == (static_cast<int32_t>(file.tellp()) - pos);
            }
            return false;
        }

        static bool truncateFile(const std::filesystem::path& filePath)
        {
            if (std::ofstream file(filePath, std::ios_base::trunc); file.is_open() && file.good()) {
                return std::filesystem::file_size(filePath) == 0;
            }
            return false;
        }

        [[nodiscard]]
        static bool checkSystemSemFile(const std::string& name)
        {
            const std::filesystem::path semaphorePath { std::filesystem::path { shmFsPath }.concat(name) };
            return std::filesystem::exists(semaphorePath);
        }

    private:

        constexpr static std::string_view sharedSegmentName { "__SHARED_MEMORY_SEGMENT_NAME_00000__1__" };
        constexpr static std::string_view shmFsPath  { R"(/dev/shm/sem.)" };
        constexpr static std::string_view semFilePath { R"(/tmp/shared_memory_channel.shm)" };

        Handle handle { InvalidHandle };

        /** FIXME: --> std::unique_ptr<T> **/
        sem_t* writeReadySemaphore { SEM_FAILED };
        std::string writeSemaphoreName {};

        /** FIXME: --> std::unique_ptr<T> **/
        sem_t* readReadySemaphore { SEM_FAILED };
        std::string readSemaphoreName {};

        using SharedDataPtr = std::unique_ptr<SharedData, decltype(&SharedMemoryChannel::closeMapping)>;
        SharedDataPtr sharedData { SharedDataPtr (nullptr, closeMapping )};
    };
}


namespace tests
{
#define DGB_PARENT LOG << "[Parent] "
#define DGB_CHILD  LOG << "[Child ] "

    using namespace std::chrono_literals;

    static void Parent()
    {
        // DGB_PARENT << "Started\n";
        SharedMemoryChannel channel;
        channel.InitializeOwner();

        channel.write();

        std::this_thread::sleep_for(500ms);

        channel.closeWriteSemaphore();
        channel.closeReadSemaphore();
        channel.unlinkMapping();

        DGB_PARENT << "Completed\n";
    }

    static void Child()
    {
        // DGB_CHILD << "Started\n";
        std::this_thread::sleep_for(500ms);

        SharedMemoryChannel channel;
        channel.InitializeClient();

        std::this_thread::sleep_for(1s);

        channel.setWriteReady();

        channel.read();

        channel.closeWriteSemaphore();
        channel.closeReadSemaphore();

        DGB_CHILD << "Completed\n";
    }

    static void multiProcessTests_WriteRead()
    {
        if (const pid_t pid = fork(); pid == 0)
            Child();
        else if (pid > 0)
            Parent();
    }
}


void ipc::shared_memory_channel::TestSharedMemoryChannel()
{
    shm_unlink("__SHARED_MEMORY_SEGMENT_NAME_00000__1__") ;

    // tests::multiProcessTests();
    tests::multiProcessTests_WriteRead();
    // SharedMemoryChannel{}.TEST();
}
