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

    void Parent()
    {
        LOG << "[Parent] Started\n";

        sem_t* semaphore = initSemaphore(semaphoreName);

        LOG << "[Parent] Waiting . . .\n";
        ::sem_wait(semaphore);
        LOG << "[Parent] Resuming . . .\n";

        closeSemaphore(semaphore, semaphoreName);;
        LOG << "[Parent] Completed\n";
    }

    void Child()
    {
        LOG << "[Child ] Started\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        sem_t* semaphore = openSemaphore(semaphoreName);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        LOG << "[Child ] sem_post() called\n";
        ::sem_post(semaphore);

        LOG << "[Child ] Completed\n";
    }

    void demo()
    {
        if (const pid_t pid = fork(); pid == 0)
            Child();
        else if (pid > 0)
            Parent();
        LOG << "Done" << std::endl;
    }
}


void ipc::semaphores::testAll()
{
    demo();
}
