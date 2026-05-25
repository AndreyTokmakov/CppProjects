/**============================================================================
Name        : FunctionObjects.hpp
Created on  : 24.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Function objects src
============================================================================**/

#ifndef FUNCTION_OBJECTS_TESTS__H_
#define FUNCTION_OBJECTS_TESTS__H_

namespace FunctionObjects
{
	void TestAll();
	namespace MemberFunctionPointer { void TestAll(); }
	namespace PassCallableToClass { void TestAll(); }
	namespace CallbackTests { void TestAll(); }
	namespace SmallFunctionWrapper { void TestAll(); }
	namespace MoveOnlyFunction { void TestAll(); }
	namespace CopyableFunction { void TestAll(); }
	namespace Static_FunctionCall_Operator { void TestAll(); }
};

#endif /* FUNCTION_OBJECTS_TESTS__H_ */