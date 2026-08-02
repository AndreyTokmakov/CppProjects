/**============================================================================
Name        : Bridge.hpp
Created on  : 08.06.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
escription : Bridge src
============================================================================**/

#ifndef BRIDGE_TESTS__H_
#define BRIDGE_TESTS__H_

namespace bridge
{
	void TestAll();
	namespace drawing_bridge { void TestAll(); }
	namespace drawing_bridge_two { void TestAll(); }
	namespace logger { void TestAll(); }
}

#endif /* BRIDGE_TESTS__H_ */
