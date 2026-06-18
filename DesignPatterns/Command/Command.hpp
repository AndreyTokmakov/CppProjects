//============================================================================
// Name        : Command.h
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Command pattern src
//============================================================================

#ifndef COMMAND_PATTERN__TESTS_H__
#define COMMAND_PATTERN__TESTS_H__

namespace Command
{
	void TestAll();
	namespace method_ptr { void TestAll(); }
	namespace filesystem_command_dispatcher { void TestAll(); }
}

#endif //COMMAND_PATTERN__TESTS_H__