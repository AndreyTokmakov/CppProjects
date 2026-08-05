/**============================================================================
Name        : Set_Value_Bases_on_Type.cpp.cpp
Created on  : 01.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Set_Value_Bases_on_Type.cpp.cpp
============================================================================**/

#include "Variant.hpp"

#include <iostream>
#include <variant>
#include <string>
#include <cstdint>

namespace
{
    template<typename ... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    constexpr auto Printer = overloaded {
        [](const bool var)         { std::cout << "Bool  : " << std::boolalpha << var << std::endl; },
        [](const double var)       { std::cout << "Double: " << var << std::endl; },
        [](const std::string& var) { std::cout << "String: " << var << std::endl; }
    };
}

namespace Variant::set_value_bases_on_type_1
{
    template<typename VarType, typename Variant, typename Item>
    constexpr bool checkTypeAndSet(Variant& variable, const Item& item)
    {
        using ItemType = std::remove_cv_t<std::remove_reference_t<Item>>;
        if constexpr  (std::is_same_v<VarType, ItemType>) {
            variable = item;
            return true;
        }
        return false;
    }

    template<typename VarType, typename Item, size_t... Idx>
    constexpr bool setValueImpl(std::index_sequence<Idx...>, VarType& variable, const Item& item) {
        return (checkTypeAndSet<std::variant_alternative_t<Idx, VarType>>(variable, item) || ... );
    }

    template<typename Variant, typename Ty>
    bool setValue(Variant& variable, const Ty& value)
    {
        using VarType = std::remove_cv_t<std::remove_reference_t<Variant>>;
        return setValueImpl<VarType>(std::make_index_sequence<std::variant_size_v<VarType>>{}, variable, value);
    }
}

namespace Variant::set_value_bases_on_type_2
{
    template<typename VarType, typename Variant, typename Item>
    constexpr bool checkTypeAndSet(Variant& variable, const Item& item)
    {
        using ItemType = std::remove_cv_t<std::remove_reference_t<Item>>;
        if constexpr  (std::is_same_v<VarType, ItemType>) {
            variable = item;
            return true;
        }
        return false;
    }

    template<typename ... Ts, typename Ty>
    constexpr bool setValue(std::variant<Ts...>& variable, const Ty& value)
    {
        return (checkTypeAndSet<Ts>(variable, value) || ... );
    }
}

namespace Variant::set_value_bases_on_type_tests
{
    // using set_value_bases_on_type_1::setValue;
    using set_value_bases_on_type_2::setValue;

    template<typename ... Ts,typename Item>
    bool testImpl(std::variant<Ts...> variable, const Item& item, const bool resultExpected)
    {
        using ItemType = std::remove_cv_t<std::remove_reference_t<Item>>;
        if (const bool result = setValue(variable, item); resultExpected != result) {
            std::cerr << "Failed to set value" << std::endl;
            return false;
        }
        if (!std::holds_alternative<ItemType>(variable)) {
            std::cerr << "std::variant<Ts...> variable initialized with wrong type" << std::endl;
            return false;
        }
        const ItemType varValue = std::get<ItemType>(variable);
        if (varValue != item) {
            std::cerr << varValue << " != " << item << std::endl;
            return false;
        }

        std::cout <<"Test passed\n";
        return true;
    }

    void debugTest()
    {
        // const bool item = true;
        // const int item = 1;
        // const double item = 12.345;
        const std::string item = "Some Text";

        std::variant<bool, double, std::string> var;
        if (const bool result = setValue(var, item); result) {
            std::visit(Printer, var);
        } else {
            std::cerr << "Failed to set value" << std::endl;
        }
    }

    void runTests()
    {
        testImpl( std::variant<bool, int32_t, double, std::string> {}, int32_t{0}, true);
        testImpl( std::variant<bool, int32_t, double, std::string> {}, false, true);
        testImpl( std::variant<bool, int32_t, double, std::string> {}, 12.23, true);
        testImpl( std::variant<bool, int32_t, double, std::string> {}, std::string { "1234"}, true);
    }
}


void Variant::Set_Value_Bases_on_Type::TestAll()
{
    using namespace set_value_bases_on_type_tests;
    // debugTest();
    runTests();
}