//============================================================================
// Name        : TestSupport.h
// Created on  : 01.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Test support classes and templates
//============================================================================

#ifndef TEST_SUPPORT_CLASSES__INCLUDE_GUARD__H
#define TEST_SUPPORT_CLASSES__INCLUDE_GUARD__H

#include <iostream>
#include <string>

using String = std::string;
using CString = const String&;

namespace TestSupport {

	class FirstType {
	protected:
		std::string value;

	public:
		FirstType(CString val) : value(val) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		FirstType(const FirstType &obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}

		virtual ~FirstType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual String getValue() const {
			return this->value;
		}

		virtual void setValue(CString val) {
			this->value = val;
		}

	//public: /** Operators reload. **/
	//	friend std::ostream& operator<<(std::ostream& stream, const FirstType& integer);
	};




	/** SecondType **/
	class SecondType {
	protected:
		String value;

	public:
		SecondType(CString val) : value(val) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		SecondType(const SecondType &obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}
		virtual ~SecondType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual String getValue() const {
			return this->value;
		}

		virtual void setValue(CString val) {
			this->value = val;
		}
	};

	class ThirdType {
	protected:
		std::string value;

	public:
		ThirdType(CString val) : value(val) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		ThirdType(const ThirdType &obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}
		virtual ~ThirdType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual String getValue() const {
			return this->value;
		}

		virtual void setValue(CString val) {
			this->value = val;
		}
	};

	class FirstTypeDerived : public FirstType {
	public:
		FirstTypeDerived(CString val) : FirstType(val) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		FirstTypeDerived(const FirstTypeDerived &obj) : FirstType(obj) {
		}

		~FirstTypeDerived() {
			std::cout << __FUNCTION__ << std::endl;
		}

		std::string getValue() const {
			return "[Derived] : " + this->value;
		}
	};
};

#endif /* TEST_SUPPORT_CLASSES__INCLUDE_GUARD__H */
