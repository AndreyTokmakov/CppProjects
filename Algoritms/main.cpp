//============================================================================
// Name        : Algoritms MAIN.cpp
// Created on  : August 12, 2019
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Algoritms
//============================================================================

#include <chrono>
#include <thread>

#include "Lists/Lists.h"
#include "Heap/Heap.h"
#include "Graphs/Graphs.h"
#include "Trees/Trees.h"
#include "Queues/Queues.h"
#include "Stack/Stack.h"
#include "Sorting/Sorting.h"
#include "Strings/Strings.h"
#include "Interviews/Interviews.h"
#include "Numeric/Numeric.h"
#include "MultiThreading/MultiThreading.h"

int main([[maybe_unused]] int argc,
		 [[maybe_unused]] char** argv)
{
	// Graphs::TEST_ALL();
	// LinkedList::TEST_ALL();
	// Heap_Algoritms::TEST_ALL();
    // Trees::TEST_ALL();
	// Queues::TEST_ALL();
	// Stack::TEST_ALL();
	// Sorting::TEST_ALL();
	// Strings::TEST_ALL();
	// Interviews::TEST_ALL();
    Numeric::TEST_ALL();
    // MultiThreading::TestAll();

	return EXIT_SUCCESS;
}
