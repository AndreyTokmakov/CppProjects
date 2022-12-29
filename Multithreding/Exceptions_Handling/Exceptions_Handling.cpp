//============================================================================
// Name        : Exceptions_Handling.cpp
// Created on  : 11.07.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Multithreading exceptions handling
//============================================================================

#include <iostream>
#include <future>
#include <execution>
#include <cmath>
#include <sstream>

#include "Exceptions_Handling.h"

namespace Exceptions_Handling
{
    void Asych_HandleException()
    {
        auto first = std::async([]() {
            throw std::bad_alloc();
        });

        auto second = std::async([]() {
            throw std::bad_alloc();
        });

        try {
            first.get();
        }
        catch (const std::exception& exception) {
            // std::cout << "catch exception from the first: " << exception.what() << std::endl;
        }
        second.wait();

        std::cout << "second has been ended" << std::endl;
    }


    double square_root(double x) {
        if (x < 0) {
            throw std::out_of_range("x < 0");
        }
        return std::sqrt(x);
    }

    void Bad_Test() {
        std::future<double> f = std::async(square_root, -1);

        [[maybe_unused]]
        double y = f.get();
    }

    //----------------------------------------------------------------------------------

    void Exception_From_Thread()
    {
        std::exception_ptr excPtr { nullptr };
        auto task = [&]()
        {
            try {
                std::stringstream str;

                // TODO: Do not compile
                // str << std::this_thread::get_id();
                throw std::runtime_error(str.str().c_str());
            } catch (...) {
                excPtr = std::current_exception();
            }
        };


        std::thread job(task);
        job.join();

        if (nullptr != excPtr)
        {
            try {
                std::rethrow_exception(excPtr);
            }
            catch (const std::exception& exc) {
                // std::cout << "Thread " << std::this_thread::get_id()<< " caught exception from thread " << exc.what() << std::endl;
            }
        }
    }
}

struct Hack
{
    unsigned int id;
};

namespace Exceptions_Asynch {

    void funcWithException() {
        std::cout << "Task started!\n";
        throw std::runtime_error("Ohh Shit!!!");
        std::cout << "Task completed!\n";
    }

    void HandleExceptionInAsynch()
    {

        try {
            auto f = std::async(std::launch::async, funcWithException);
            // f.wait();
            f.get();
        }
        catch (...) {
            std::cout << "Some exception raised!\n";
        }

        std::cout << "Done!\n";
    }
}


void Exceptions_Handling::TEST_ALL()
{
    // Asych_HandleException();
    // Bad_Test();

    // Exception_From_Thread();

    Exceptions_Asynch::HandleExceptionInAsynch();
};