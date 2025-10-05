//============================================================================
// Name        : Utilities.h
// Created on  : 10.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities libraries src
//============================================================================

#ifndef UTILITIES_INCLUDE_GUARD__H
#define UTILITIES_INCLUDE_GUARD__H

namespace Utilities
{
	void TestAll();

	namespace NullPtr_T { void TestAll(); }
	namespace Invoke  { void TestAll(); }
	namespace CompilerVersion { void TestAll(); }
	namespace ScopeExit { void TestAll(); }

};

#endif /* UTILITIES_INCLUDE_GUARD__H */
