/**============================================================================
Name        : CRTP_InvokeMethods.cpp
Created on  : 02.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP_InvokeMethods.cpp
============================================================================**/

#include <iostream>
#include <array>
#include <variant>

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
    using ItemsTuple = std::tuple<Base<Args>...>;

    using ItemsVariant = std::variant<ItemOne, ItemTwo>;

    void test_with_tuple()
    {
        ItemsTuple<ItemOne, ItemTwo> itemsTuple { ItemOne{}, ItemTwo{} };
        std::apply([]<typename... T>(Base<T>... items) { (items.call(), ...); },itemsTuple);
    }

    void test_with_variant()
    {
        std::array<ItemsVariant, 2> itemsVar = { ItemsVariant { ItemOne{} }, ItemsVariant { ItemTwo{} } };
        for (auto& elem : itemsVar)
        {
            std::visit([]<typename T>(Base<T> item) { item.call(); }, elem);
        }
    }
}

void CRTP_InvokeMethods_Test()
{
    // CRTP_InvokeMethods::test_with_tuple();
    CRTP_InvokeMethods::test_with_variant();
}