//============================================================================
// Name        : Utilities.cpp
// Created on  : 01.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities class
//============================================================================

#include "Integer.h"
#include <utility>
#include <iostream>
#include <cassert>

Integer::Integer() : value(0) {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

Integer::Integer(int val) : value(val) {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

Integer::Integer(const Integer& obj) {
	PrintInfo(__FUNCTION__, "[Copy contructor]. (", obj.value, ")");
	this->value = obj.value;
}

Integer::Integer(Integer&& obj) noexcept : value(std::exchange(obj.value, 0)) {
	PrintInfo(__FUNCTION__, " [Move  contructor]. (", obj.value, ")");
}

Integer::~Integer() {
	PrintInfo(__FUNCTION__, "(", this->value, ")");
}

int Integer::getValue() const noexcept {
	return this->value;
}

void Integer::printInfo() const noexcept {
	std::cout << "Info : Utilities value = " << this->value << std::endl;
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

const Integer operator+(const Integer& left, int int_value) {
	PrintInfo("[2] Utilities& operator+ (", left.value, " + ", int_value, ")");
	return Integer(left.value + int_value);
}

const Integer operator+(const Integer& left, const Integer& right) {
	PrintInfo("[3] Utilities& operator+ (", left.value, " + ", right.value, ")");
	return Integer(left.value + right.value);
}

const Integer operator-(const Integer& left, int int_value) {
	PrintInfo(__FUNCTION__, " (", left.value, " - ", int_value, ")");
	return Integer(left.value - int_value);
}

const Integer operator-(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " - ", right.value, ")");
	return Integer(left.value - right.value);
}

Integer& operator+=(Integer& left, const Integer& right) {
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
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right, ") int");
	return left.value < right;
}

bool operator>(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value > right.value;
}

bool operator>(const int left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left, " vs ", right.value, ") int");
	return left > right.value;
}

bool operator<=(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value <= right.value;
}

bool operator>=(const Integer& left, const Integer& right) {
	PrintInfo(__FUNCTION__, " (", left.value, " vs ", right.value, ")");
	return left.value >= right.value;
}

const Integer& operator+(const Integer& integer) {
	PrintInfo("[1] Utilities& operator+ (", integer.value, ")");
	return Integer(+integer.value);
}

const Integer operator-(const Integer& integer) {
	return Integer(-integer.value);
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


//////////////////////////////////////////// TESTS //////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                 Integer_Class_Testting                                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#define ASSERT(expression, text) try {                                     \
									assert(expression);                    \
								 } catch (...) {                           \
									std::cout << text << std::endl; throw  \
								 }                                         \
*/
namespace Integer_Class_Testting {

	struct IntegerHash {
		std::size_t operator()(const Integer& s) const noexcept {
			return std::hash<int>{}(s.getValue());
		}
	};

	void Constructor() {
		Integer var(123);
		assert(123 == var.getValue());

		var.increment();
		assert(124 == var.getValue());

		// ERROR. Since Utilities(int val) is explicit
#if 0
		Utilities int2 = 12;
#endif
	}

	void Increment_Postfix() {
		Integer var(111);
		assert(111 == var.getValue());

		var++;
		assert(112 == var.getValue());
	}

	void Increment_Prefix() {
		Integer var(111);
		assert(111 == var.getValue());

		++var;
		assert(112 == var.getValue());
	}

	void Assignment_Integer() {
		Integer var1(11), var2(22);

		assert(11 == var1.getValue());
		assert(22 == var2.getValue());

		var1 = var2;

		assert(22 == var1.getValue());
		assert(22 == var2.getValue());

		var1 = var1;
	}

	void Assignment_Int() {
		Integer var(11);
		assert(11 == var.getValue());

		int value = 321;
		var = value;

		assert(value == var.getValue());
	}

	void CompareOperatorTest_Greater() {
		Integer var1(11), var2(12);

		assert(11 == var1.getValue());
		assert(12 == var2.getValue());
		assert(var2 > var1);
		assert(var1 < var2);
	}

	void CompareOperatorTest_Greater_int() {
		Integer var1(11);
		int var2 = 12;

		assert(11 == var1.getValue());
		assert(12 == var2);
		assert(var2 > var1);
		assert(var1 < var2);
	}

	void CompareOperatorTest_GreaterEqual() {
		{
			Integer var1(11), var2(11);

			assert(11 == var1.getValue());
			assert(11 == var2.getValue());
			assert(var2 >= var1);
			assert(var1 <= var2);
		}
		{
			Integer var1(11), var2(13);

			assert(11 == var1.getValue());
			assert(13 == var2.getValue());
			assert(var2 >= var1);
			assert(var1 <= var2);
		}
	}

	void Plus_OperatorTest() {
		{
			Integer var1(3), var2(4);
			assert(3 == var1.getValue());
			assert(4 == var2.getValue());

			Integer result = var1 + var2;
			assert(7 == result.getValue());
		}
		{
			Integer var1(3);
			assert(3 == var1.getValue());

			Integer result = var1 + 44;
			assert(47 == result.getValue());
		}
	}

	void CompareOperatorTest_Minus() {
		{
			Integer var1(3), var2(4);
			assert(3 == var1.getValue());
			assert(4 == var2.getValue());

			Integer result = var1 - var2;
			assert(-1 == result.getValue());
		}
		{
			Integer var1(13);
			assert(13 == var1.getValue());

			Integer result = var1 - 4;
			assert(9 == result.getValue());
		}
	}

	void CopyConstructor() {

		const auto func1 = [](const Integer integer) {
			std::cout << integer << std::endl;
		};

		const auto func2 = [](int v) {
			Integer var(v);
			var.setValue(v);
			return var;
		};

		std::cout << "-------------------------------------- Test1 -----------------------------" << std::endl;
		{
			Integer var(13);
			func1(var);
			assert(13 == var.getValue());
		}

		std::cout << "\n-------------------------------------- Test2 -----------------------------" << std::endl;
		{
			Integer var = func2(123);
			assert(123 == var.getValue());
		}

		std::cout << "\n-------------------------------------- Test3 -----------------------------" << std::endl;
		{
			Integer var1(1);
			Integer var2 = var1;
			assert(1 == var2.getValue());
		}

		std::cout << "\n-------------------------------------- Test4 -----------------------------" << std::endl;
		{
			Integer var1(1);
			Integer var2(var1);
			assert(1 == var2.getValue());
		}
	}

	Integer foo(Integer i) {
		return i;
	}

	void SumTest() {
		Integer var1(1);
		Integer var2(3);

		Integer int3 = var1 + var2;

		std::cout << int3 << std::endl;
	}

	void MoveAssignmentOperator() {
		std::cout << "Test 0:" << std::endl;
		{
			Integer var1(13);
			{
				Integer var2(23);
				var2 = std::move(var1);
			}
		}

		std::cout << "\n\nTest 1:" << std::endl;
		{
			Integer v = std::move(Integer(23));
		}

		std::cout << "\n\nTest 2:" << std::endl;
		{
			Integer var1(122);
			Integer var2 = foo(std::move(var1));
		}

		std::cout << "\n\nTest 3:" << std::endl;
		{
			Integer var1(122);
			Integer var2 = foo(var1);
		}
	}

	void Hash_Integer() {

		IntegerHash hash_fn;
		Integer integer(123);
		std::cout << "Utilities hash = " << hash_fn(integer) << std::endl;
	}

}

namespace Integer_Class_Testting {
	void TEST_ALL() {

		std::cout << sizeof(Integer) << std::endl;

		// Constructor();
		// CopyConstructor();

		// Increment_Postfix();
		// Increment_Prefix();

		// Assignment_Int();
		// Assignment_Integer();
		// MoveAssignmentOperator();


		// CompareOperatorTest_Greater();
		// CompareOperatorTest_Greater_int();
		// CompareOperatorTest_GreaterEqual();

		// Plus_OperatorTest();
		// CompareOperatorTest_Minus();

		// SumTest();

		// Hash_Integer();
	}
}