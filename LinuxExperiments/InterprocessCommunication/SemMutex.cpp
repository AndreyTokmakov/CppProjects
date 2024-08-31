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
#include <chrono>

namespace SemMutex
{
    constexpr std::string_view semaphoreName { "Read_Semaphore_Name_One" };

    int error(const std::string &func)
    {
        std::cerr << func << " failed. Error = " << errno << std::endl;
        return errno;
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
    void ChildProcess()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        std::optional<sem_t*> semaphore = openSemaphore();

        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
            LockGuard lock {semaphore.value()};
            std::cout << "ChildProcess  " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250));
            lock.~LockGuard();
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    }

    void ParentProcess()
    {
        std::optional<sem_t*> semaphore = createSemaphore();
        std::this_thread::sleep_for(std::chrono::milliseconds (10));

        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
            LockGuard lock {semaphore.value()};
            std::cout << "ParentProcess " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250));
            if (i == 5)std::this_thread::sleep_for(std::chrono::seconds (3));
            lock.~LockGuard();
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }

        closeSemaphore(semaphore);
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
}