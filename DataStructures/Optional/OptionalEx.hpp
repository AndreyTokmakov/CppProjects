/**============================================================================
Name        : Optional.hpp
Created on  : 13.08.2020
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Optional (custom implementation) src
============================================================================**/


#ifndef CPPPROJECTS_OPTIONALEX_HPP
#define CPPPROJECTS_OPTIONALEX_HPP

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <utility>
#include <array>

template<class T>
class OptionalEx
{
    using data_type  = T;
    using pointer    = T*;
    using reference  = T&;

    static constexpr std::size_t Size = sizeof(data_type);
    alignas(data_type) std::array<std::byte, Size> storage {};

    bool has_value = false;

    pointer ptr() noexcept {
        return std::launder(reinterpret_cast<pointer>(storage.data()));
    }

public:
    constexpr OptionalEx() noexcept = default;

    template<typename ... Types>
    constexpr OptionalEx(Types&& ... args): has_value { true }  {
        ::new (storage.data()) data_type(std::forward<Types>(args)...);
    }

    constexpr OptionalEx(const OptionalEx& rhs)
            requires std::is_copy_constructible_v<data_type>
    {
        if (rhs.has_value) {
            emplace(*rhs.ptr());
        }
    }

    constexpr OptionalEx(OptionalEx&& rhs)
            noexcept requires std::is_move_constructible_v<data_type>
    {
        if (rhs.has_value) {
            emplace(std::move(*rhs.ptr()));
        }
    }

    template<class... Args>
    constexpr reference emplace(Args&&... args)
    {
        reset();
        ::new (storage.data()) data_type(std::forward<Args>(args)...);
        has_value = true;
        return *ptr();
    }

    constexpr void reset() noexcept
    {
        if (has_value) {
            std::destroy_at(ptr());           // C++20 helper
            has_value = false;
        }
    }

    constexpr explicit operator bool() const noexcept
    {
        return has_value;
    }

    [[nodiscard]]
    constexpr reference operator*() noexcept
    {
        if (!has_value) {
            throw std::bad_optional_access{};
        }
        return *ptr();
    }

    [[nodiscard]]
    constexpr reference value() &
    {
        if (!has_value) {
            throw std::bad_optional_access{};
        }
        return *ptr();                        // UB-safe: launder внутри
    }

    constexpr ~OptionalEx()
    {
        reset();
    }
};

#endif //CPPPROJECTS_OPTIONALEX_HPP