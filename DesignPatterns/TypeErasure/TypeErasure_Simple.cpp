/**============================================================================
Name        : TypeErasure_Simple.cpp
Created on  : 27.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeErasure_Simple.cpp
============================================================================**/

#include "TypeErasure.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <print>

namespace
{
    struct Object
    {
        template <typename T>
        explicit Object(T&& obj): implPtr { std::make_unique<Model<T>>(std::forward<T>(obj)) } {
        }

        [[nodiscard]]
        std::string getName() const {
            return implPtr->getName();
        }

        struct Interface
        {
            [[nodiscard]]
            virtual std::string getName() const = 0;

            virtual ~Interface() = default;
        };

        template< typename T>
        struct Model final : Interface
        {
            explicit Model(const T& t) : object(t) {
                /** **/
            }
            explicit Model(T&& obj) : object { std::forward<T>(obj) } {
                /** **/
            }

            [[nodiscard]]
            std::string getName() const override {
                return object.getName();
            }

        private:
            T object;
        };

        std::unique_ptr<Interface> implPtr { nullptr };
    };
}

namespace TypeErasure_Simple_Tests
{
    void printName(Object& obj)
    {
        std::println("name: {}", obj.getName());
    }

    struct Bar
    {
        [[nodiscard]]
        static std::string getName() {
            return __PRETTY_FUNCTION__;
        }
    };

    struct Foo
    {
        [[nodiscard]]
        std::string getName() const {
            return __PRETTY_FUNCTION__;
        }
    };
}

void TypeErasure_Simple::Test()
{
    using namespace TypeErasure_Simple_Tests;

    Object obj1 { Bar() }, obj2 { Foo() };

    printName(obj1);
    printName(obj2);
}