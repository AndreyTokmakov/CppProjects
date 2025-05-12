//============================================================================
// Name        : Integer.cpp
// Created on  : 01.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Integer class 
//============================================================================

#include "Integer.h"
#include <iostream>
#include <memory>

Integer::Integer() : value(0) {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

Integer::Integer(int val) : value(val) {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

Integer::Integer(const Integer &obj) {
	PrintInfo(__FUNCTION__, "[Copy contructor]. (", obj.value, ")");
	this->value = obj.value;
}

Integer::Integer(Integer && obj) noexcept : value(std::exchange(obj.value, 0)) {
	PrintInfo(__FUNCTION__, " [Move  contructor]. (", obj.value, ")");
}

Integer::~Integer() {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

int Integer::getValue() const noexcept {
	return this->value;
}

void Integer::printInfo() const noexcept {
	std::cout << "Info : Integer value = " << this->value << std::endl;
}

void Integer::setValue(int val) noexcept {
	this->value = val;
}

void Integer::increment(void) noexcept {
	this->value++;
}

std::ostream& operator<<(std::ostream& os, const Integer& integer) {
	os << integer.value;
	return os;
}

Integer operator+(const Integer& left, int int_value) {
	PrintInfo(__FUNCTION__, " (", left.value, " + ", int_value, ")");
	return Integer(left.value + int_value);
}

Integer operator+(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " + ", right.value, ")");
	return Integer(left.value + right.value);
}

Integer operator-(const Integer& left, int int_value) {
	PrintInfo(__FUNCTION__, " (", left.value, " - ", int_value, ")");
	return Integer(left.value - int_value);
}

Integer operator-(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " - ", right.value, ")");
	return Integer(left.value - right.value);
}

Integer operator+=(Integer& left, const Integer& right) {
	left.value += right.value;
	return left;
}

bool operator==(const Integer& left, const Integer& right) {
	return left.value == right.value;
}

bool operator<(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value < right.value;
}

bool operator<(const Integer& left, const int right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right, ")");
	return left.value < right;
}

bool operator>(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value > right.value;
}

bool operator<=(const Integer & left, const Integer & right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value <= right.value;
}

bool operator>=(const Integer & left, const Integer & right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value >= right.value;
}

Integer operator+(const Integer& integer) {
	PrintInfo(__FUNCTION__, " (", integer.value, ")");
	return Integer(+integer.value);
}

Integer operator-(const Integer& integer) {
	return Integer(-integer.value);
}

Integer operator++(Integer& integer) {
	integer.value++;
	return integer;
}

Integer operator++(Integer& integer, int) {
	Integer oldValue(integer.value);
	integer.value++;
	return oldValue;
}

Integer operator--(Integer& integer) {
	integer.value--;
	return integer;
}

Integer operator--(Integer& integer, int) {
	Integer oldValue(integer.value);
	integer.value--;
	return oldValue;
}
