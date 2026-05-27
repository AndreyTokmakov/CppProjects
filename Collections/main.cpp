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
#include "Inplace_Vector/InplaceVector.hpp"
#include "Map/Map.h"
#include "MultiMap/MultiMap.h"
#include "Flat_Map/FlatMap.h"
#include "UnorderedSet/UnorderedSet.h"
#include "UnorderedMap/UnorderedMap.h"
#include "PriorityQueue/PriorityQueue.h"
#include "ForwardList/ForwardList.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
	// Array::TestAll();
	// List::TestAll();
	// ForwardList::TestAll();
	// Deque::TestAll();
	// Stack::TestAll();
	// Heap::TestAll();
	// Vector::TestAll();
	// inplace_vector::TestAll();
	// Valarray::TestAll();
	// Map::TestAll();
	// FlatMap::TestAll();
	// MultiMap::TestAll();
    UnorderedMap::TestAll();
    // Set::TestAll();
    // UnorderedSet::TestAll();
	// Queue::TestAll();
	// PriorityQueue::TestAll();
	// Custom_Vector::TestAll();
}
