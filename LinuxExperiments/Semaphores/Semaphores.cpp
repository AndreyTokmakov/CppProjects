/**============================================================================
Name        : Semaphores.cpp
Created on  : 12.07.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Semaphores.cpp
============================================================================**/

#include "Semaphores.hpp"


#include "Random.hpp"
#include "DateTimeUtilities.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <syncstream>
#include "semaphore.h"
#include <cstdint>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define DEBUG(Stream) std::osyncstream { Stream }  << '[' << getCurrentTime() << "] "
#define LOG  DEBUG(std::cout)
#define ERR  DEBUG(std::cerr)

namespace
{
    using namespace utilities::random;
    using namespace DateTimeUtilities;

    constexpr int32_t InvalidHandle { -1 };

    constexpr std::string_view semaphoreName { "test_semaphore_1" };

    sem_t* initSemaphore(const std::string_view name)
    {
        sem_t* semaphore = ::sem_open(name.data(), O_CREAT, 0777, 0);
        if (SEM_FAILED == semaphore) {
            throw std::runtime_error("sem_open() failed");
        }
        return semaphore;
    };

    sem_t* openSemaphore(const std::string_view name)
    {
        sem_t* semaphore  = ::sem_open(name.data(), O_CREAT);
        if (SEM_FAILED == semaphore) {
            throw std::runtime_error("sem_open() failed");
        }
        return semaphore;
    };

    void closeSemaphore(sem_t* semaphore, const std::string_view name)
    {
        if (InvalidHandle == ::sem_close(semaphore)) {
            ERR << "sem_close() failed. Error = " << errno << std::endl;
        }
        if (InvalidHandle == ::sem_unlink(name.data())) {
            ERR << "sem_unlink() failed. Error = " << errno << std::endl;
        }
    };

    int getSemaphoreValue(sem_t* semaphore)
    {
        int val {0};
        if (0 == ::sem_getvalue(semaphore, &val)) {
            return val;
        }
        return -1;
    }

    void Parent()
    {
        LOG << "[Parent] Started\n";

        sem_t* semaphore = initSemaphore(semaphoreName);

        LOG << "[Parent] Sem value: " << getSemaphoreValue(semaphore) << ". Waiting ....\n";

        ::sem_wait(semaphore);

        LOG << "[Parent] Sem value: " << getSemaphoreValue(semaphore) << ". Resuming . . .\n";

        closeSemaphore(semaphore, semaphoreName);;
        LOG << "[Parent] Completed\n";
    }

    void Child()
    {
        LOG << "[Child ] Started\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        sem_t* semaphore = openSemaphore(semaphoreName);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        LOG << "[Child ] Sem value: " << getSemaphoreValue(semaphore) << " before ::sem_post()\n";
        ::sem_post(semaphore);

        LOG << "[Child ] Completed: " << getSemaphoreValue(semaphore) << " ::sem_post() called\n";
    }

    void demo()
    {
        if (const pid_t pid = fork(); pid == 0)
            Child();
        else if (pid > 0)
            Parent();
    }
}


void ipc::semaphores::testAll()
{
    demo();
    /*
    [2026-07-22 20:55:41.619451] [Parent] Started
    [2026-07-22 20:55:41.619596] [Parent] Sem value: 0. Waiting ....
    [2026-07-22 20:55:41.619547] [Child ] Started
    [2026-07-22 20:55:43.720395] [Child ] Sem value: 0 before ::sem_post()
    [2026-07-22 20:55:43.720490] [Child ] Completed: 1 ::sem_post() called
    [2026-07-22 20:55:43.720556] [Parent] Sem value: 0. Resuming . . .
    [2026-07-22 20:55:43.720711] [Parent] Completed
    */
}
