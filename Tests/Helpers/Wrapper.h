/**============================================================================
Name        : Wrapper.h
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Long
============================================================================**/

#ifndef CPPPROJECTS_HELPERS__H
#define CPPPROJECTS_HELPERS__H

#include <iostream>
#include <utility>

namespace Helpers
{
    template<typename Type, bool debug = true>
    struct Wrapper
    {
        using value_type = Type;
        value_type value {0};

        explicit Wrapper(value_type val = value_type{}) : value {val}
        {
            if constexpr (debug)
                std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
        }

        Wrapper(const Wrapper &obj): value { obj.value}
        {
            if constexpr (debug)
                std::cout << __FUNCTION__ << "(" << value << ") [Copy constructor]\n";
        }

        Wrapper(Wrapper &&obj) noexcept: value {std::exchange(obj.value, 0)}
        {
            if constexpr (debug)
                std::cout << __FUNCTION__ << "(" << value << ") [Move constructor]\n";
        }

        inline void setValue(value_type v) noexcept {
            value = v;
        }

        [[nodiscard]]
        inline value_type getValue() const noexcept {
            return value;
        }

        ~Wrapper()
        {
            if constexpr (debug)
                std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
        }

        Wrapper &operator=(const Wrapper &right)
        {
            if constexpr (debug) {
                std::cout << "[Copy assignment] (" << value << " -> " << right.value << ")" << std::endl;
            }
            if (&right != this)
                value = right.value;
            return *this;
        }

        Wrapper &operator=(value_type val)
        {
            if constexpr (debug) {
                std::cout << "[Copy assignment (from long)]" << std::endl;
            }
            this->value = val;
            return *this;
        }

        Wrapper &operator=(Wrapper &&right) noexcept
        {
            if constexpr (debug) {
                std::cout << "[Move assignment operator]" << std::endl;
            }
            if (this != &right) {
                this->value = std::exchange(right.value, 0);
            }
            return *this;
        }

        Wrapper &operator*(const Wrapper &right) noexcept {
            this->value *= right.value;
            return *this;
        }

        /** Postfix increment: **/
        Wrapper operator++(value_type) {
            decltype(auto) prev = *this;
            ++value;
            return prev;
        }

        /** Prefix increment: **/
        Wrapper operator++() {
            ++value;
            return *this;
        }

    public:

        friend Wrapper operator*(const Wrapper &left, const value_type v) noexcept {
            return LongWrapper(left.value * v);
        }

        friend std::ostream& operator<<(std::ostream& os, const Wrapper& right) {
            os << right.value;
            return os;
        }

        friend Wrapper operator+(const Wrapper& left, const Wrapper& right) {
            return Integer(left.value + right.value);
        }

        friend Wrapper operator+=(Wrapper& left, const Wrapper& right) {
            left.value += right.value;
            return left;
        }

        friend bool operator==(const Wrapper& left, const Wrapper& right) {
            return left.value == right.value;
        }

        friend bool operator<(const Wrapper& left, const Wrapper& right) {
            return left.value < right.value;
        }

        friend bool operator>(const Wrapper& left, const Wrapper& right) {
            return left.value > right.value;
        }

        friend Wrapper operator++(Wrapper& left, value_type) {
            Wrapper oldValue(left.value);
            ++left.value;
            return oldValue;
        }

        friend Wrapper operator--(Wrapper& left, value_type) {
            Wrapper oldValue(left.value);
            left.value--;
            return oldValue;
        }
    };

    using Long = Wrapper<long, true>;
    using Integer = Wrapper<int, true>;

};

#endif //CPPPROJECTS_HELPERS__H
