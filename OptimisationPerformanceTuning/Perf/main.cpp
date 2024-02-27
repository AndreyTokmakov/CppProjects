/**============================================================================
Name        : main.cpp
Created on  : 23.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <cstdint>
#include <iostream>
#include <vector>
#include <thread>


struct Application
{
    uint64_t counter = 0;

    void funcOne()
    {
        for (int i = 0; i < 1'000; ++i)
        {
            ++counter;
            std::this_thread::sleep_for(std::chrono::nanoseconds( 10));
        }
    }

    void funcTwo()
    {
        for (int i = 0; i < 1'000 * 10; ++i)
        {
            ++counter;
            std::this_thread::sleep_for(std::chrono::nanoseconds (1));
        }
    }

    void start()
    {
        for (int i = 0; i < 10; ++i) {
            funcOne();
        }

        for (int i = 0; i < 10; ++i) {
            funcTwo();
        }

        std::cout << counter << std::endl;
    }
};

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    std::cout << "Running test app" << std::endl;

    Application app {};
    app.start();


    return EXIT_SUCCESS;
}

