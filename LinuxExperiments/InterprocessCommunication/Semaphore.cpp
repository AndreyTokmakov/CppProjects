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

namespace Semaphore
{

    constexpr std::string_view semaphoreName { "my_named_semaphore_test"};

    void getSemaphoreValue(sem_t *sem ) {
        int value {0};
        int result = sem_getvalue(sem,&value);
        if (-1 == result) {
            std::cout << "sem_getvalue() failed. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Semaphore counter value = " << value << std::endl;
        }
    }

    void MultiprocessClient_CreateWaitAndSet() {
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT );
        if (SEM_FAILED == sem) {
            std::cout << "sem_open() failed. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Semaphore created" << std::endl;
        }

        getSemaphoreValue(sem);
        // int result = sem_post(sem);
        // getSemaphoreValue(sem);
        // timespec ts {10, 0};

        std::cout << "Waiting for semaphore...." << std::endl;
        int result = sem_wait(sem);
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
        sem_t *sem = sem_open(semaphoreName.data(), O_CREAT | O_EXCL );
        if (SEM_FAILED == sem) {
            std::cout << "sem_open() failed. Error = " << errno << std::endl;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            sem_post(&semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    }

    void* pong(void*) {
        int local;
        for (int i = 0; i < 10; i++) {
            sem_wait(&semaphore);
            local = counter;
            std::cout << "Pong: " << local << std::endl;
            local--;
            counter = local;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            sem_post(&semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    }

    void MultithreadTest_LinuxStyle() {
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

void Semaphore::TestAll()
{

    MultithreadTest_LinuxStyle();
};
