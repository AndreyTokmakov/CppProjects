#pragma once

#include <iostream>

template<bool debug = true>
class Integer
{
    int value { 0 };

public:

    Integer()
    {
        if constexpr (debug)
            std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
    }

    explicit Integer(int val) : value { val }
    {
        if constexpr (debug)
            std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
    }

    Integer(const Integer &obj)
    {
        if constexpr (debug)
            std::cout << __FUNCTION__ << " [Copy contructor]. (" << this->value << ")" << std::endl;
        this->value = obj.value;
    }

    ~Integer()
    {
        if constexpr (debug)
            std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
    }

    [[nodiscard]]
    int getValue() const {
        return this->value;
    }

    void printInfo() const {
        std::cout << "Info : Integer value = " << this->value << std::endl;
    }

    void setValue(int val) {
        this->value = val;
    }


public: /** Operators reload. **/

    friend std::ostream& operator<<(std::ostream& os, const Integer& integer) {
        os << integer.value;
        return os;
    }

    friend Integer operator+(const Integer& left, const Integer& right) {
        return Integer(left.value + right.value);
    }

    friend Integer operator+=(Integer& left, const Integer& right) {
        left.value += right.value;
        return left;
    }

    friend bool operator==(const Integer& left, const Integer& right) {
        return left.value == right.value;
    }

    friend bool operator<(const Integer& left, const Integer& right) {
        return left.value < right.value;
    }

    friend bool operator>(const Integer& left, const Integer& right) {
        return left.value > right.value;
    }

    Integer operator=(const Integer& right) {
        if (this == &right) {
            return *this;
        }
        value = right.value;
        return *this;
    }

    friend Integer operator+(const Integer& integer) {
        return integer.value;
    }

    friend Integer operator-(const Integer& integer) {
        return Integer(-integer.value);
    }

    friend Integer operator++(Integer& integer) {
        integer.value++;
        return integer;
    }

    friend Integer operator++(Integer& integer, int) {
        Integer oldValue(integer.value);
        ++integer.value;
        return oldValue;
    }

    friend Integer operator--(Integer& integer) {
        integer.value--;
        return integer;
    }

    friend Integer operator--(Integer& integer, int) {
        Integer oldValue(integer.value);
        integer.value--;
        return oldValue;
    }
};
