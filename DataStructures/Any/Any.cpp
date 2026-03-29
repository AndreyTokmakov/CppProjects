/**============================================================================
Name        : Any.cpp
Created on  : 29.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Any.cpp
============================================================================**/

#include "Any.hpp"

#include <iostream>
#include <format>
#include <print>
#include <memory>

namespace
{
    class Any
    {
        struct Interface
        {
            virtual ~Interface() = default;
            virtual void func() const noexcept  = 0;
        };

        template <typename Ty>
        class Model : public Interface
        {
            Ty object;

        public:

            explicit Model(const Ty& obj) noexcept : object { obj } {
            }

            void func() const noexcept override {
                object.func();
            }
        };

        std::unique_ptr<Interface> implPtr;

    public:

        template <typename T>
        explicit Any(T&& obj) noexcept :
            implPtr { std::make_unique<Model<T>>(std::forward<T>(obj)) } {
        }

        void func() const noexcept {
            implPtr->func();
        }
    };
}


namespace tests
{
    struct X
    {
        void func() const noexcept
        {
            std::cout << "X::func()" << std::endl;
        }
    };

    void test()
    {
        const Any any {X {}};
        any.func();
    }
}

void any::TestAll()
{
    tests::test();

}
