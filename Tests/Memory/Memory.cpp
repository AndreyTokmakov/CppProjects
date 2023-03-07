/**============================================================================
Name        : Memory.cpp
Created on  : 18.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Memory test
============================================================================**/

#include "Memory.h"

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

#include "../Helpers/Utilities.h"
#include "../Helpers/Long.h"
#include "../Helpers/Object.h"

namespace Memory
{
    struct Object
    {
        Object() { std::cout << "Object()" << std::endl; }
        ~Object() { std::cout << "~Object()" << std::endl; }

        Object(const Object&) { std::cout << "Object(const Object& obj)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object(Object&& obj) noexcept" << std::endl; }

        Object& operator==(const Object&) {
            std::cout << "Object& operator==(const Object&)" << std::endl;
            return *this;
        }

        Object& operator==(Object&&) noexcept {
            std::cout << "Object& operator==(Object&&) noexcept" << std::endl;
            return *this;
        }
    };

    class BadClass
    {
    private:
        Memory::Object* obj = new Object();
        // std::unique_ptr<Memory::Object> obj { std::make_unique<Memory::Object>() };

    public:

        BadClass() {
            throw 1;
        }

        ~BadClass() {
            delete obj;
        }
    };

    void CleanUP_Exception_Test()
    {
        try {
            BadClass b;
        }
        catch (...) {
            std::cout << "Ops" << std::endl;
        }
    }
}


namespace Memory
{
    using session = int;

    auto session_factory(int id) {
        return std::make_shared<session>(id);
    }

    // https://ibob.bg/blog/2023/01/01/tracking-shared-ptr-leaks/
    void SharedPtrLeak()
    {
        std::shared_ptr<session> leak;

        std::vector<std::weak_ptr<session>> registry;

        constexpr int N = 20;
        srand(unsigned(std::time(nullptr)));
        auto i_to_leak = rand() % (2 * N);

        // std::cout << "i_to_leak = " << i_to_leak << std::endl;

        for (int i = 0; i < N; ++i) {
            auto sptr = session_factory(i);
            registry.push_back(sptr);
            if (i == i_to_leak) {
                leak = sptr;
                // std::cout << "Expect " << leak << " to leak\n";
            }
        }

        for (auto& w : registry) {
            if (w.use_count()) {
                std::cout << "found a leak in " << w.lock() << "\n";
            }
        }

        std::cout << "Done\n";
    }
}

namespace Memory::UniquePtrExperiments
{
    using namespace Helpers;

    void PointerToObjectOnStack()
    {
        auto longDeleter = [](Long* ptr) {
            std::cout << "Skip deletion for Long(" << ptr << ")\n";
            //delete ptr;
        };

        Long l1(10);

        {
            std::unique_ptr<Long, decltype(longDeleter)> lPtr{&l1, longDeleter};
        }

        std::cout << "Done\n";

    }
}

void Memory::TestAll()
{

    // CleanUP_Exception_Test();
    // SharedPtrLeak();

    UniquePtrExperiments::PointerToObjectOnStack();
}
