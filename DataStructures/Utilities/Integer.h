/**============================================================================
Name        : Utilities.h
Created on  : 01.04.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities class
============================================================================**/

#ifndef INTEGER_INCLUDE_GUARD__H
#define INTEGER_INCLUDE_GUARD__H

#include <iostream>
#include <utility>
#define DEBUG_OUTPUT

template<typename ...Args>
void PrintInfo(Args&&... args) {
#ifdef DEBUG_OUTPUT
	(std::cout << ... << std::forward<Args>(args)) << std::endl;
#endif
}

#define DEBUG(...) PrintInfo(__VA_ARGS__) \

/** Integet class. **/
class Integer {
protected:
	int value;

public:
	Integer();
	explicit Integer(int val);
	Integer(const Integer& obj);
	Integer(Integer&& obj) noexcept;

	virtual ~Integer();

	virtual int getValue() const noexcept;
	virtual void setValue(int val) noexcept;

	virtual void printInfo() const noexcept;
	virtual void increment(void) noexcept;


public: /** Operators reload. **/
	friend std::ostream& operator<<(std::ostream& stream,
		const Integer& integer);

	friend const Integer operator+(const Integer& left,
		int int_value);

	friend const Integer operator+(const Integer& left,
		const Integer& right);

	friend const Integer operator-(const Integer& left,
		int int_value);

	friend const Integer operator-(const Integer& left,
		const Integer& right);

	friend Integer& operator+=(Integer& left,
		const Integer& right);

	friend bool operator==(const Integer& left,
		const Integer& right);

	friend bool operator<(const Integer& left,
		const Integer& right);

	friend bool operator<(const Integer& left,
		const int right);

	friend bool operator>(const Integer& left,
		const Integer& right);

	friend bool operator>(const int left,
		const Integer& right);

	friend bool operator<=(const Integer& left,
		const Integer& right);

	friend bool operator>=(const Integer& left,
		const Integer& right);

	Integer& operator=(const Integer& right) {
		std::cout << "[Copy assignment operator from Utilities -> Utilities]" << std::endl;
		if (&right != this) {
			value = right.value;
		}
		return *this;
	}

	Integer& operator=(int val) {
		std::cout << "[Copy assignment operator from int -> Utilities]" << std::endl;
		this->value = val;
		return *this;
	}

	Integer& operator=(Integer&& right) noexcept {
		std::cout << "[Move assignment operator]" << std::endl;
		if (this != &right) {
			this->value = std::exchange(right.value, 0);
		}
		return *this;
	}

	friend const Integer& operator+(const Integer& integer);
	friend const Integer operator-(const Integer& integer);
	friend const Integer& operator++(Integer& integer);
	friend const Integer operator++(Integer& integer, int);
	friend const Integer& operator--(Integer& integer);
	friend const Integer operator--(Integer& integer, int);
};


namespace Integer_Class_Testting {
	void TEST_ALL();
}

#endif // !INTEGER_INCLUDE_GUARD__H