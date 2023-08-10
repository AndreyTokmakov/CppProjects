/**============================================================================
Name        : Long.cpp
Created on  : 10.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Long class
============================================================================**/

#include "Long.h"

#include <iostream>
#include <utility>

Long::Long(int val) : value {val}
{
    std::cout << "Long(" << value << ")\n";
}

Long::Long(const Long &obj)
{
    this->value = obj.value;
    std::cout << "Long(" << value << ") [Copy constructor]\n";
}

Long::Long(Long &&obj) noexcept: value { std::exchange(obj.value, 0) }
{
    std::cout << "Long(" << value << ") [Move constructor]\n";
}

inline void Long::setValue(long v) noexcept
{
    value = v;
}

[[nodiscard]]
inline long Long::getValue() const noexcept
{
    return value;
}

Long::~Long()
{
    std::cout << "~Long(" << value << ")\n";
}

Long& Long::operator=(const Long& right)
 {
    std::cout << "Long& Long::operator=(const Long& right)(" << right.value << ")\n";
    if (&right != this)
        value = right.value;
    return *this;
}

Long& Long::operator=(long val) {
    std::cout << "[Copy assignment (from long)]" << std::endl;
    this->value = val;
    return *this;
}

Long& Long::operator=(Long &&right) noexcept
{
    if (this != &right)
        this->value = std::exchange(right.value, 0);
    return *this;
}

Long & Long::operator*(const Long &right) noexcept {
    this->value *= right.value;
    return *this;
}

/** Postfix increment: **/
Long Long::operator++(int) {
    Long prev = *this;
    ++value;
    return prev;
}

/** Prefix increment: **/
Long Long::operator++() {
    ++value;
    return *this;
}

Long operator*(const Long &left, long v) noexcept
{
    return Long(left.value * v);
}

std::ostream &operator<<(std::ostream &stream, const Long &l)
{
    stream << l.value;
    return stream;
}