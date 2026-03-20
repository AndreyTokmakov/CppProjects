/**============================================================================
Name        : MiniSet.cpp
Created on  : 20.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MiniSet.cpp
============================================================================**/

#include "MiniSet.hpp"

#include <iostream>
#include <array>
#include <source_location>

namespace
{
    constexpr uint8_t charBit { 8 };
    constexpr uint16_t uintOne { 1 };
    constexpr uint16_t maxValue { sizeof(uint16_t) * charBit };

    [[nodiscard]]
    constexpr bool is_set(const uint16_t mask, const uint16_t index) noexcept {
        return index < maxValue && (mask & (uintOne << index));
    }

    [[nodiscard]]
    constexpr bool is_not_set(const uint16_t mask, const uint16_t index) noexcept {
        return !is_set(mask, index);
    }

    constexpr void unset_bit(uint16_t& mask, const uint16_t bit) noexcept {
        if (bit < maxValue) {
            mask &= ~(uintOne << bit);
        }
    };

    constexpr void set_bit(uint16_t& mask, const uint16_t bit) noexcept {
        if (bit < maxValue) {
            mask |= (uintOne << bit);
        }
    }
}


namespace
{
    struct Iterator
    {
        using value_type = uint16_t;
        using mask_type  = uint16_t;

        constexpr explicit Iterator(const mask_type mask) noexcept: remainingMask { mask } {
            updateIndex();
        }

        constexpr Iterator& operator++() noexcept
        {
            if (remainingMask == 0)
                return *this;

            // Remove current bit
            remainingMask &= (remainingMask - 1);

            updateIndex();
            return *this;
        }

        constexpr value_type operator*() const noexcept
        {
            return currentIndex;
        }

        [[nodiscard]]
        constexpr bool operator==(const Iterator& other) const noexcept
        {
            return remainingMask == other.remainingMask;
        }

        [[nodiscard]]
        constexpr bool operator!=(const Iterator& other) const noexcept
        {
            return !(*this == other);
        }

    private:

        mask_type remainingMask { 0 };
        value_type currentIndex { 0 };

        constexpr void updateIndex() noexcept
        {
            if (remainingMask == 0)
                return;
            if constexpr (__cpp_lib_bitops >= 201907L) {
                currentIndex = std::countr_zero(remainingMask);
            }
            else {
                currentIndex = __builtin_ctz(remainingMask);
            }
        }
    };

    struct Set
    {
        using size_type  = uint16_t;
        using value_type = uint16_t;

        constexpr Set() = default;

        void insert(const value_type val)
        {
            if (val >= maxValue) {
                return;
            }
            set_bit(mask, val);
        }

        constexpr void erase(const value_type val) {
            unset_bit(mask, val);
        }

        [[nodiscard]]
        constexpr size_type size() const noexcept
        {
            if constexpr (__cpp_lib_bitops >= 201907L) {
                return std::popcount(mask);
            }
            else {
                return __builtin_popcount(mask);
            }
        }

        [[nodiscard]]
        constexpr bool contains(const value_type val) const noexcept {
            return is_set(mask, val);
        }

        [[nodiscard]]
        constexpr Iterator begin() const noexcept {
            return Iterator { mask };
        }

        [[nodiscard]]
        constexpr Iterator end() const noexcept {
            return Iterator { 0 };
        }

    private:

        value_type mask { 0 };
    };

    [[maybe_unused]]
    std::ostream& operator<<(std::ostream& os, const Set& set)
    {
        for (const auto i : set)
            os << i << " ";
        return os ;
    }
}


void Assert(const bool ok, const std::source_location location = std::source_location::current())
{
    if (!ok) {
        std::cerr << "Error: " << location.file_name() << "\n\t\t[" << location.function_name()
            << ":" << location.line() << "]\n";
        std::terminate();
    }
}


void mini_set::TestAll()
{
    constexpr std::array<int, 3> data1 {2, 8, 12};
    constexpr std::array<int, 2> data2 {2, 12};

    Set set {};
    for (const int v: data1) {
        set.insert(v);
    }
    for (const int v: data1) {
        Assert(set.contains(v));
    }
    for (uint32_t idx = 0; const auto i : set) {
        Assert(data1[idx++] == i);
    }

    Assert(3 == set.size());
    set.erase(8);

    for (const int v: data2) {
        Assert(set.contains(v));
    }
    for (uint32_t idx = 0; const auto i : set) {
        Assert(data2[idx++] == i);
    }

    Assert(2 == set.size());
}
