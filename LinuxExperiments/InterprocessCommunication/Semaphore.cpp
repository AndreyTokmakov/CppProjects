//============================================================================
// Name        : Semaphore.h
// Created on  : 12.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Semaphore
//============================================================================

#include "Semaphore.h"

#include "../common.h"
#include <semaphore.h>


#include <iostream>
#include <string_view>
#include <thread>
#include <optional>
#include <format>
#include <chrono>

namespace
{
    void printErrorInfo(int errorCode)
    {
        if (EACCES == errorCode)
            std::cout << "EACCES: The semaphore exists, but the caller does not have permission to open it\n";
        else if (EEXIST == errorCode)
            std::cout << "EEXIST: Both O_CREAT and O_EXCL were specified in oflag, but a semaphore with this name already exists\n";
        else if (EINVAL == errorCode)
            std::cout << "EINVAL: value was greater than SEM_VALUE_MAX\n";
        else if (EINVAL == errorCode)
            std::cout << "EINVAL: name consists of just \"/\", followed by no other     characters.\n";
        else if (EMFILE == errorCode)
            std::cout << "EMFILE: The per-process limit on the number of open file descriptors has been reached.\n";
        else if (ENAMETOOLONG == errorCode)
            std::cout << "ENAMETOOLONG: name was too long.\n";
        else if (ENFILE == errorCode)
            std::cout << "ENFILE: The system-wide limit on the total number of open files has been reached\n";
        else if (ENOENT == errorCode)
            std::cout << "ENOENT: The O_CREAT flag was not specified in oflag and nosemaphore with this name exists; "
                         "or, O_CREAT was specified, but name wasn't well formed.\n";
        else if (ENOMEM == errorCode)
            std::cout << "ENOMEM: Insufficient memory\n";
        else
            std::cout << "Error: " << errorCode << std::endl;
    }

    struct CurrentTime
    {
        const std::chrono::time_point<std::chrono::high_resolution_clock> now { std::chrono::system_clock::now() };
    };

    [[maybe_unused]]
    std::ostream& operator<<(std::ostream& stream, const CurrentTime& time)
    {
        stream << std::format("{:%d-%m-%Y %H:%M:%OS}", time.now);
        return stream;
    }

    struct LOG final : public std::stringstream
    {
        constexpr static inline std::string_view FORMAT { "[{:%d-%m-%Y %H:%M:%OS}] "};

        ~LOG() override
        {
            std::cout << std::format(FORMAT, std::chrono::system_clock::now());
            std::cout << rdbuf();
            std::cout.flush();
        }
    };
}


namespace Semaphore::Tests
{
    constexpr std::string_view semaphoreName { "my_named_semaphore_test"};

    void getSemaphoreValue(sem_t *sem ) {
        int value {0};
        int result = sem_getvalue(sem,&value);
        if (-1 == result) {
            std::cout << "sem_getvalue() failed" << std::endl;
            printErrorInfo(errno);
            return;
        } else {
            std::cout << "Semaphore counter value = " << value << std::endl;
        }
    }

    void MultiprocessClient_CreateWaitAndSet()
    {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
        if (SEM_FAILED == sem) {
            std::cout << "sem_getvalue() failed" << std::endl;
            printErrorInfo(errno);
            return;
        } else {
            std::cout << "Semaphore created" << std::endl;
        }

        getSemaphoreValue(sem);
        // int result = sem_post(sem);
        // getSemaphoreValue(sem);
        // timespec ts {10, 0};

        std::cout << "Waiting for semaphore...." << std::endl;

        [[maybe_unused]]
        timespec timeout {10, 0};

        [[maybe_unused]]
        int result = sem_wait(sem);
        // const int result = sem_timedwait(sem, &timeout);
        std::cout << "Semaphore is taken. Waiting for it to be released." << std::endl;

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

    void MultiprocessServer_GetAndReleaseSemaphore() {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            std::cout << "sem_getvalue() failed" << std::endl;
            printErrorInfo(errno);
            return;
        } else {
            std::cout << "Semaphore is taken" << std::endl;
        }

        /*
        std::cout << "Semaphore is taken. Waiting for it to be released." << std::endl;
        if (sem_wait(sem) < 0 ) {
            std::cout << "ERROR : sem_wait" << std::endl;
        }
        */

        sem_post(sem);
        std::cout << "Semaphore is released." << std::endl;
    }

    //---------------------------------------------------------------------

    sem_t semaphore;
    int counter = 0;

    void* ping(void*) {
        int local;
        for (int i = 0; i < 10; i++) {
            sem_wait(&semaphore);
            local = counter;
            std::cout << "Ping: " << local << std::endl;
            local++;
            counter = local;
            std::this_thread::sleep_for(std::chrono::milliseconds(250u));
            sem_post(&semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds (10u));
        }
        return nullptr;
    }


    void* pong(void*) {
        int local;
        for (int i = 0; i < 10; i++) {
            sem_wait(&semaphore);
            local = counter;
            std::cout << "Pong: " << local << std::endl;
            local--;
            counter = local;
            std::this_thread::sleep_for(std::chrono::milliseconds(250u));
            sem_post(&semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds (10u));
        }
        return nullptr;
    }

    void MultithreadTest_LinuxStyle()
    {
        pthread_t thread1, thread2;

        sem_init(&semaphore, 0, 1);

        pthread_create(&thread1, nullptr, ping, nullptr);
        pthread_create(&thread2, nullptr, pong, nullptr);

        //pthread_join(thread1, nullptr);
        pthread_join(thread2, nullptr);

        sem_destroy(&semaphore);
        printf("== %d", counter);
    }
};

namespace Semaphore::Multiprocess
{
    constexpr std::string_view semaphoreName { "my_named_semaphore_test" };

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

    void CreateSemaphore()
    {
        sem_t *sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, 0);
        if (SEM_FAILED == sem) {
            std::cout << "sem_getvalue() failed" << std::endl;
            printErrorInfo(errno);
            return;
        } else {
            std::cout << "Semaphore created" << std::endl;
        }

        [[maybe_unused]]
        timespec timeout {10, 0};
        while (true)
        {
            std::cout << "Waiting for semaphore...." << std::endl;

            [[maybe_unused]]
            int result = sem_wait(sem);
            std::cout << "Semaphore is taken. Waiting for it to be released." << std::endl;

            int value {0};
            result = sem_getvalue(sem,&value);
            std::cout << "value = " << value << std::endl;
        }
        closeSemaphore(sem);
    }

    void SetSemaphore()
    {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            std::cout << "sem_getvalue() failed" << std::endl;
            printErrorInfo(errno);
            return;
        } else {
            std::cout << "Semaphore is taken" << std::endl;
        }

        //int value {0};
        //int result = sem_getvalue(sem,&value);

        sem_post(sem);
        std::cout << "Semaphore is released." << std::endl;
    }
}

namespace Semaphore::MultiprocessTest
{
    constexpr std::string_view semaphoreOneName { "Read_Semaphore_Name_One" };
    constexpr std::string_view semaphoreTwoName { "Read_Semaphore_Name_Two" };

    int error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
    }

    std::optional<sem_t*> CreateSemaphore(std::string_view semaphoreName,
                                          uint32_t value)
    {
        sem_t *sem = ::sem_open(semaphoreName.data(), O_CREAT, 0777, value);
        if (SEM_FAILED == sem) {
            error("sem_open()");
            return std::nullopt;
        } else { // FIXME: remove
            // std::cout << semaphoreName << " semaphore created\n";
            return sem;
        }
    }

    std::optional<sem_t*> OpenSemaphore(std::string_view semaphoreName)
    {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            error("sem_open");
            return std::nullopt;
        } else { // FIXME: remove
            // std::cout << semaphoreName << " semaphore opened\n";
            return sem;
        }
    }

    void closeSharedMem(sem_t *sem,
                        std::string_view semaphoreName)
    {
        if (0 != sem_close(sem)) {
            error("sem_close()");
        }
        if (0 != sem_unlink(semaphoreName.data())) {
            error("sem_unlink()");
        }
        else {
            // std::cout << semaphoreName << " semaphore closed" << std::endl;
        }
    }

    void ProcessOne()
    {
        //LOG{} << "ProcessOne. Creating .....\n";
        std::optional<sem_t*> semOne = CreateSemaphore(semaphoreOneName, 0);
        if (!semOne)
            return;
        std::optional<sem_t*> semTwo = CreateSemaphore(semaphoreTwoName, 0);
        if (!semTwo)
            return;

        //LOG{} << "ProcessOne. Created. ID: " << getpid() << std::endl;
        for (int i = 0; i < 10; ++i)
        {
            // std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250u));
            // LOG{} << "ProcessOne. Releasing semOne\n";
            ::sem_post(semOne.value());

            LOG{} << "ProcessOne. Waiting for TWO\n";

            [[maybe_unused]]
            const int value  = ::sem_wait(semTwo.value());
            // LOG{} << "ProcessOne. TWO ok. value = " << value << "\n";
        }

        closeSharedMem(semTwo.value(), semaphoreTwoName);
        closeSharedMem(semOne.value(), semaphoreOneName);
    }

    void ProcessTwo()
    {
        //LOG{} << "ProcessTwo. Creating .....\n";
        std::this_thread::sleep_for(std::chrono::milliseconds (100u));

        std::optional<sem_t*> semOne = OpenSemaphore(semaphoreOneName);
        if (!semOne)
            return;
        std::optional<sem_t*> semTwo = OpenSemaphore(semaphoreTwoName);
        if (!semTwo)
            return;

        //LOG{} << "ProcessTwo. Created. ID: " << getpid() << std::endl;
        for (int i = 0; i < 10; ++i)
        {
            // std::cout << std::endl;
            // LOG{} << "ProcessTwo. Waiting for ONE\n";

            [[maybe_unused]]
            const int value = ::sem_wait(semOne.value());
            // LOG{} << "ProcessTwo. ONE ok. value = " << value << "\n";

            std::this_thread::sleep_for(std::chrono::milliseconds (250u));
            // LOG{} << "ProcessTwo. Releasing semTwo\n";
            ::sem_post(semTwo.value());
        }
    }

    void CreateTwoProcesses()
    {
        if (const pid_t pid = fork(); pid == 0) { /** Child **/
            ProcessTwo();
            return;
        }
        else if (pid > 0) { /** Parent **/
            ProcessOne();
        }
        else {
            std::cout << "Unable to create child process" << std::endl;
        }
    }
}

void Semaphore::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{

    /*
    if (!params.empty())
    {
        const int value = atoi(params.front().data());
        if (0 == value) {
            MultiprocessClient_CreateWaitAndSet();
        } else if (1 == value)
        {
            MultiprocessServer_GetAndReleaseSemaphore();
        }
    }
    */


    // MultithreadTest_LinuxStyle();


    // Multiprocess::CreateSemaphore();
    // Multiprocess::SetSemaphore();

    MultiprocessTest::CreateTwoProcesses();
};
