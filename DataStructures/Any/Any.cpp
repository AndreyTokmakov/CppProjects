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
#include <vector>
#include <atomic>


namespace any
{
    struct TypeId
    {
        using type_id_t = uint16_t;

        template<typename>
        [[nodiscard]]
        static type_id_t getId()
        {
            static const type_id_t typeID = getUniqueId();
            return typeID;
        }

    private:

        [[nodiscard]]
        static type_id_t getUniqueId() noexcept
        {
            static std::atomic<uint32_t> currentTypeId { 0 };
            return currentTypeId.fetch_add(1, std::memory_order::relaxed);
        }
    };

    struct Any
    {
        TypeId::type_id_t actualTypeId {};

        Any() = default;

        template <typename Ty>
        explicit Any(Ty&& item)
        {
            std::cout << typeid(item).name() << " ==> " << TypeId::getId<Ty>()  << std::endl;
        }

        template <typename Ty>
        [[nodiscard]]
        bool containsType() const noexcept
        {
            return actualTypeId == TypeId::getId<Ty>();
        }

        template<typename Ty>
        Any& operator=(const Ty& item)
        {
            std::cout << typeid(item).name() << " ==> " << TypeId::getId<Ty>() << std::endl;

            constexpr size_t typeSize = sizeof(std::remove_reference_t<Ty>);
            placeholder.resize(typeSize);
            placeholder.shrink_to_fit();

            // std::cout << placeholder.size() << std::endl;
            auto* str = std::construct_at(reinterpret_cast<Ty*>(placeholder.data()), item);
            // std::cout << *str << std::endl;

            return *this;
        }

        std::vector<uint8_t> placeholder;
    };

    void test()
    {
        /*
        {
            Any any {int{}};
            std::cout << std::boolalpha << any.containsType<int>() << ", " << any.containsType<double>() << std::endl;
        }*/

        Any any;

        any = std::string{ "123456"};
        //any = int{};
        //any = std::string{};
    }
}

void any::TestAll()
{
    any::test();
}
