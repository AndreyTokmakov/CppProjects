/**============================================================================
Name        : CRTP_InvokeMethods.cpp
Created on  : 02.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP_InvokeMethods.cpp
============================================================================**/

#include <iostream>

namespace CRTP_InvokeMethods
{
    template <typename Derived>
    struct Base
    {
        void call() {
            static_cast<Derived *>(this)->test();
        }
    };

    struct ItemOne : public Base<ItemOne>
    {
        void test() {
            std::cout << "ItemOne::test()\n";
        }
    };

    struct ItemTwo : public Base<ItemTwo>
    {
        void test() {
            std::cout << "ItemTwo::test()\n";
        }
    };

    template <typename... Args>
    using Items = std::tuple<Base<Args>...>;

    void test()
    {
        Items<ItemOne, ItemTwo> items { ItemOne{}, ItemTwo{} };
        std::apply([]<typename... T>(Base<T>... items) { (items.call(), ...); },items);
    }
}

void CRTP_InvokeMethods_Test()
{
    CRTP_InvokeMethods::test();
}