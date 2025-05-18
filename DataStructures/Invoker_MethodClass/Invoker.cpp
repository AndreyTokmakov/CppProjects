/**============================================================================
Name        : Invoker.cpp
Created on  : 18.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Invoker.cpp
============================================================================**/

#include "Invoker.h"

#include <iostream>
#include <functional>

namespace
{
    template<typename F>
    concept FunctionPointer = std::is_member_function_pointer_v<F>;

    template<typename Derived>
    struct BuilderBase
    {
        template<typename... T>
        Derived& when(FunctionPointer auto func, T &&... params) {
            Derived& self = get_self();
            std::invoke(func, self, std::forward<T>(params)...);
            return self;
        }

        Derived& when(auto action)
        {
            Derived& self = get_self();
            action(self);
            return self;
        }

        inline Derived& get_self() noexcept
        {
            return static_cast<Derived&>(*this);
        }

    };
}

namespace
{
    struct DataBuilder: BuilderBase<DataBuilder>
    {
        void getInfo(int v) const {
            std::cout << "Data::getInfo(" << v << ")" << std::endl;
        }

        void getInfoStr(std::string_view str) const {
            std::cout << "Data::getInfo(" << str << ")" << std::endl;
        }

        void invokeInfo()
        {
            std::invoke(&DataBuilder::getInfo, this, 2);
        }
    };
}




void Invoker::TestAll()
{
    DataBuilder data;

    // data.invokeInfo();

    auto func = [](const DataBuilder& obj) {
        obj.getInfoStr("world");
    };

    data.when(&DataBuilder::getInfo, 6)
            .when(&DataBuilder::getInfoStr, "hello")
            .when(func);
}