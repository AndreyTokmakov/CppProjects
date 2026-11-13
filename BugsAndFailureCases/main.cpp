/**============================================================================
Name        : main.cpp
Created on  : 10.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BugsAndFauilreCases
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include <thread>
#include <chrono>
#include <syncstream>


namespace init_list_bad_virtual_call
{
    struct Specialist
    {
        virtual std::string getSpeciality() = 0;
        virtual ~Specialist() = default;
    };

    struct Doctor
    {
        Doctor(const std::string& name, Specialist& specialist) :
            title ( specialist.getSpeciality() + " Doctor")
        {
        }

        std::string title;
    };

    // INFO: specialist.getSpeciality() above will be called
    //       YET Doctor is not created at that moment
    struct CppDoctor : Doctor, Specialist
    {
        CppDoctor(): Doctor("Max", *this) {
        }

        std::string getSpeciality() override {
            return "C++";
        }
    };

    void test()
    {
        CppDoctor cppDoctor;
        std::cout << cppDoctor.title << std::endl;
    }
}


int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);



    return EXIT_SUCCESS;
}

