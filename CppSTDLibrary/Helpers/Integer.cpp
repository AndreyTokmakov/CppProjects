/**============================================================================
Name        : Integer.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Integer.h"
#include <iostream>

Integer::Integer() : value(0) {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

Integer::Integer(int val) : value(val) {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

Integer::Integer(const Integer &obj) {
    std::cout << __FUNCTION__ << " [Copy constructor]. (" << obj.value << ")" << std::endl;
    this->value = obj.value;
}

Integer::Integer(Integer && obj) noexcept : value(std::exchange(obj.value, 0)) {
    std::cout << __FUNCTION__ << " [Move constructor]. (" << obj.value << ")" << std::endl;
}

Integer::~Integer() {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

int Integer::getValue() const noexcept  {
    return this->value;
}

void Integer::printInfo() const noexcept  {
    std::cout << "Info : Integer value = " << this->value << std::endl;
}

void Integer::setValue(int val) noexcept  {
    this->value = val;
}

void Integer::increment(void) noexcept  {
    this->value++;
}

std::ostream& operator<<(std::ostream& os, const Integer& integer) {
    os << integer.value;
    return os;
}

const Integer operator+(const Integer& left, const Integer& right) {
    return Integer(left.value + right.value);
}

Integer& operator+=(Integer& left, const Integer& right) {
    left.value += right.value;
    return left;
}

bool operator==(const Integer& left, const Integer& right) {
    return left.value == right.value;
}

bool operator<(const Integer& left, const Integer& right) {
    return left.value < right.value;
}

bool operator>(const Integer& left, const Integer& right) {
    return left.value > right.value;
}

const Integer operator+(const Integer& integer) {
    return Integer(+integer.value);
}

const Integer operator-(const Integer& integer) {
    return Integer(-integer.value);
}

int operator+(const Integer& integer, int v) {
    return integer.getValue() + v;
}

int operator-(const Integer& integer, int v) {
    return integer.getValue() - v;
}

const Integer& operator++(Integer& integer) {
    integer.value++;
    return integer;
}

const Integer operator++(Integer& integer, int) {
    Integer oldValue(integer.value);
    integer.value++;
    return oldValue;
}

const Integer& operator--(Integer& integer) {
    integer.value--;
    return integer;
}

const Integer operator--(Integer& integer, int) {
    Integer oldValue(integer.value);
    integer.value--;
    return oldValue;
}
