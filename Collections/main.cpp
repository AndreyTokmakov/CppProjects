/**============================================================================
Name        : CollectionsTests.cpp
Created on  : 12.08.2019
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CollectionsTests
============================================================================**/

#include <iostream>

#include "Custom_Vector/Custom_Vector.h"
#include "Queue/Queue.h"
#include "Valarray/Valarray.h"
#include "Heap/Heap.h"
#include "Deque/Deque.h"
#include "List/List.h"
#include "Array/Array.h"
#include "Stack/Stack.h"
#include "Set/Set.h"
#include "Vector/Vector.h"
#include "Map/Map.h"
#include "MultiMap/MultiMap.h"
#include "UnorderedSet/UnorderedSet.h"
#include "UnorderedMap/UnorderedMap.h"
#include "PriorityQueue/PriorityQueue.h"
#include "ForwardList/ForwardList.h"

#define TEST(nspace) nspace::TEST_ALL();

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
	// TEST(Array);
	// List::TestAll();
	// TEST(ForwardList);
	// TEST(Deque);
	// TEST(Stack);
	// TEST(Heap);
	// Vector::TestAll();
	// TEST(Valarray);
	// TEST(Map);
	// MultiMap::TestAll();
    // TEST(UnorderedMap);
    // TEST(Set);
    TEST(UnorderedSet);
	// TEST(Queue);
	// PriorityQueue::TestAll();
	// TEST(Custom_Vector);
}
