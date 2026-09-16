/**============================================================================
Name        : Memory.hpp
Created on  : 14.05.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AlignedStackAllocator
============================================================================**/

#ifndef MEMORY_TESTS_INCLUDE_GUARD__H
#define MEMORY_TESTS_INCLUDE_GUARD__H

#include <iostream>

namespace memory
{
	void TestAll();

	namespace aligned_stack_allocator_basic { void TestAll(); }
	namespace aligned_stack_allocator_extended { void TestAll(); }
	namespace custom_stack_allocator { void TestAll(); }
	namespace object_pool_stack_fixed_size { void TestAll(); }

	namespace alignment { void TestAll(); }
	namespace launder { void TestAll(); }
	namespace memory_pool { void TestAll(); }
	namespace memory_usage_monitor { void TestAll(); }
};

#endif // !MEMORY_TESTS_INCLUDE_GUARD__H
