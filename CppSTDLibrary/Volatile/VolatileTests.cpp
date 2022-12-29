//============================================================================
// Name        : VolatileTests.cpp
// Created on  : 04.06.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : VolatileTests
//============================================================================

#include "VolatileTests.h"


#include <future>
#include <iostream>
#include <string>
#include <string_view>

namespace VolatileTests {

    /*
    int ready;
    int buffer[100];

    void do_init(int i) {
        buffer[i] = i;
        asm volatile ("" ::: "memory");
        ready = i;
    }

    void send(int v) {
    }

    void do_wait() {
        while(ready)
            asm volatile ("" ::: "memory");
        send(buffer[ready]);
    }
    */

    bool ready {false};

    void producer() {
        for (int i = 0; i< 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // std::cout << i << std::endl;
        }

        std::cout << "Set ready True!\n";
        ready = true;
    }

    void wait() {
        while (!ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // std::cout << "Not ready!\n";
        }

        std::cout << "OK\n";
    }

    void Test() {
        auto future1 = std::async(producer);
        auto future2 = std::async(wait);

        future1.wait();
        future2.wait();
    }
}

void VolatileTests::TestAll()
{
    Test();
}