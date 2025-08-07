/**============================================================================
Name        : Bind.cpp
Created on  : 13.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Bind.cpp
============================================================================**/

#include <functional>
#include <iostream>
#include <memory>

namespace Bind
{
    struct Utilities
    {
        void Info() {
            std::cout << "Utilities::Info()" << std::endl;
        }

        void printer(const std::string& text) {
            std::cout << __FUNCTION__ << ". Text: " << text << std::endl;
        }
        void printer_two_params(std::string_view prefix, std::string_view text) {
            std::cout << prefix << " : " << text << std::endl;
        }
    };

    struct Worker
    {
        void doWork(const std::string& text) {
            std::cout << "Input text: " << text << std::endl;
        }
    };

    int minus(int a, int b) {
        return a - b;
    }

    void auto_printer(const std::string& text1, const std::string& text2) {
        std::cout << text1 << " . " << text2 << std::endl;
    };

    template<typename ...Args>
    void fold_printer(Args&&... args) {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }
}

namespace Bind
{
    void Bind()
    {
        auto func1 = std::bind(auto_printer, "One", "Test1");
        auto func2 = std::bind(auto_printer, "Two", std::placeholders::_1);
        auto func3 = std::bind(&Utilities::printer, new Utilities(), std::placeholders::_1);

        func1();
        func2("Test2");
        func3("Three");
    }

    void Bind_ObjectMethod()
    {
        std::unique_ptr<Worker> worker { std::make_unique<Worker>()};
        Worker worker2 {};

        auto func = std::bind(&Worker::doWork, worker.get(), std::placeholders::_1);
        auto func1 = std::bind(&Worker::doWork, worker.get(), "Predefined Text");
        auto func_ref = std::bind(&Worker::doWork, &worker2, "Predefined Text (Ref)");

        func("World");   // Input text: World
        func1();         // Input text: Predefined Text
        func_ref();      // Input text: Predefined Text (Ref)
    }

    void Bind_Front()
    {
        auto fifty_minus = std::bind_front(minus, 50);
        std::cout << fifty_minus(3) << std::endl; // --> 47
    }

    void Bind_Back()
    {
        auto f = [](int a, int b, int c, int d, int e, int f) {
            std::cout << a << " " << b << " " << c << " " <<
                      d << " " << e << " " << f << "\n";
        };

        // auto bound = std::bind_back(f, 1, 2, 3);
        // bound(4,5,6); // prints: 4 5 6 1 2 3
    }
}

void Bind_Tests()
{
    using namespace Bind;

    // Bind();
    Bind_ObjectMethod();
    // Bind_Front();
    // Bind_Back();
}