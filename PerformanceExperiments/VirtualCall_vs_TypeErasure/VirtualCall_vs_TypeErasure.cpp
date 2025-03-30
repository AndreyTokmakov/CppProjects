/**============================================================================
Name        : VirtualCall_vs_TypeErasure.cpp
Created on  : 30.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : VirtualCall_vs_TypeErasure.cpp
============================================================================**/

#include "VirtualCall_vs_TypeErasure.h"

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

namespace
{
    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start { std::chrono::high_resolution_clock::now() };

        explicit ScopedTimer(std::string_view info) : benchmarkName { info } {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };

    constexpr int testIterCount = 100'000'000;
}


namespace VirtualCall_vs_TypeErasure::TypeErasure
{
    struct Wrapper
    {
        struct Interface
        {
            virtual void call() = 0;
            virtual ~Interface() = default;
        };

        template<class Impl>
        struct Intermediate: Interface
        {
            explicit Intermediate(Impl&& obj) noexcept : object { std::forward<Impl>(obj) } {
            }

            void call() final {
                return object.call();
            }

        private:
            Impl object {};
        };

    public:

        template<typename Impl>
        explicit Wrapper(Impl&& obj): ptrImpl {
            std::make_shared<struct Intermediate<Impl>>(std::forward<Impl>(obj)) } {
        }

        void call() {
            ptrImpl->call();
        }

    private:

        std::shared_ptr<Interface> ptrImpl { nullptr };
    };

    struct One { void call() { /*std::cout << "One\n"; */} };
    struct Two { void call() { /*std::cout << "Two\n"; */} };
    struct Three { void call() { /*std::cout << "Three\n"; */} };
    struct Four { void call() { /*std::cout << "Four\n"; */} };
    struct Five { void call() { /*std::cout << "Five\n"; */} };

    std::vector<Wrapper> getData()
    {
        std::vector<Wrapper> vec {
            Wrapper { One{} },
            Wrapper { Two{} },
            Wrapper { Three{} },
            Wrapper { Four{} },
            Wrapper { Five{} }
        };
        return vec;
    }


    void test()
    {
        std::vector<Wrapper> vec = getData();
        for (auto & obj: vec) {
            obj.call();
        }
    }

    void benchmark()
    {
        std::vector<Wrapper> vec = getData();

        ScopedTimer timer { "TypeErasure" };
        for (int i = 0; i < testIterCount; ++i)
        {
            for (auto & obj: vec) {
                obj.call();
            }
        }
    }
}

namespace VirtualCall_vs_TypeErasure::VirtualCall
{
    struct Interface
    {
        virtual void call() = 0;
        virtual ~Interface() = default;
    };

    struct One : Interface { void call() override { /*std::cout << "One\n";*/ } };
    struct Two : Interface { void call() override { /*std::cout << "Two\n";*/ } };
    struct Three : Interface { void call() override { /*std::cout << "Three\n";*/ } };
    struct Four : Interface { void call() override { /*std::cout << "Four\n";*/ } };
    struct Five : Interface { void call() override { /*std::cout << "Five\n";*/ } };

    std::vector<std::unique_ptr<Interface>> getData()
    {
        std::vector<std::unique_ptr<Interface>> vec;
        {
            vec.emplace_back(std::make_unique<One>());
            vec.emplace_back(std::make_unique<Two>());
            vec.emplace_back(std::make_unique<Three>());
            vec.emplace_back(std::make_unique<Four>());
            vec.emplace_back(std::make_unique<Five>());
        }
        return vec;
    }

    void test()
    {
        std::vector<std::unique_ptr<Interface>> vec = getData();
        for (auto &obj: vec) {
            obj->call();
        }
    }

    void benchmark()
    {
        std::vector<std::unique_ptr<Interface>> vec = getData();

        ScopedTimer timer { "VirtualCall" };
        for (int i = 0; i < testIterCount; ++i)
        {
            for (auto &obj: vec) {
                obj->call();
            }
        }
    }
}



void VirtualCall_vs_TypeErasure::benchmark()
{
    //TypeErasure::test();
    //VirtualCall::test();

    TypeErasure::benchmark();
    VirtualCall::benchmark();

    // TypeErasure   :  0.935436 seconds.
    // VirtualCall   :  0.799691 seconds.
}