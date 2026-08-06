/**============================================================================
Name        : Flyweight.hpp
Created on  : 12.04.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Flyweight pattern src
============================================================================**/

#ifndef FLYWEIGHT_PATTERN__TESTS_H__
#define FLYWEIGHT_PATTERN__TESTS_H__

namespace flyweight
{
	void TestAll();

	namespace icon { void Test(); }
	namespace character { void Test(); }
}

#endif // !(FLYWEIGHT_PATTERN__TESTS_H__)