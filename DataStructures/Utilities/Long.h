/**============================================================================
Name        : Long.h
Created on  : 10.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Long class
============================================================================**/

#ifndef CPPPROJECTS_LONG_H
#define CPPPROJECTS_LONG_H

#include <iostream>

struct Long final
{
    long value {0};

    explicit Long(int val = 0);

    Long(const Long &obj);
    Long(Long &&obj) noexcept;

    inline void setValue(long v) noexcept;

    [[nodiscard]]
    inline long getValue() const noexcept;

    ~Long();

    Long& operator=(const Long &right);
    Long& operator=(long val);
    Long& operator=(Long && right) noexcept;
    Long& operator*(const Long& right) noexcept;

    /** Postfix increment: **/
    Long operator++(int);

    /** Prefix increment: **/
    Long operator++();

    friend Long operator*(const Long &left, long v) noexcept;

    friend std::ostream &operator<<(std::ostream &stream, const Long &l);
};

#endif //CPPPROJECTS_LONG_H
