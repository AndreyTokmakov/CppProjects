/**============================================================================
Name        : TypeErasure_Two.cpp
Created on  : 01.05.20225
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeErasure_Two
============================================================================**/

#include "TypeErasure.hpp"

#include <iostream>
#include <array>
#include <memory>
#include <utility>
#include <concepts>

namespace
{
    template <typename T>
    concept HaveCallMethod = requires (T t) {
        { t.call() } -> std::same_as<void>;
    };

    struct Interface
    {
        virtual void call_impl() = 0;
        virtual ~Interface() = default;
    };

    template <typename T>
    struct ItemProxy final : public Interface
    {
        explicit ItemProxy(T t) : impl { std::forward<T>(t) } {
        }

        void call_impl() override {
            impl.call();
        }
    private:
        T impl;
    };

    struct ItemBase
    {
        template <HaveCallMethod T>
        explicit ItemBase(T t): ptrItem { std::make_unique<ItemProxy<T>>(std::forward<T>(t)) } {
        }

        void call() const
        {
            ptrItem->call_impl();
        }

    private:
        std::unique_ptr<Interface> ptrItem;
    };
}


namespace Implementation
{
    struct RealItemOne {
        std::string name;
        void call() const { std::cout << name << std::endl; }
    };

    struct RealItemTwo
    {
        std::string name;
        void call() const { std::cout << name << std::endl; }
    };
}

void TypeErasure_Two::Test()
{
    using namespace Implementation;

    const std::array<ItemBase, 2> items {
        ItemBase { RealItemOne{"RealItemOne"} },
        ItemBase { RealItemTwo{"RealItemTwo"} }
    };
    for (auto &item : items) {
        item.call();
    }
}