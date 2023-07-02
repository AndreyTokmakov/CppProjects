//============================================================================
// Name        : CollectionsTests.cpp
// Created on  : August 12, 2019
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Collections study and src project
//============================================================================

#include <iostream>

#include <map>
#include <vector>
#include <list>
#include <tuple>
#include <algorithm>
#include <set>
#include <unordered_map>

#include <string>
#include <string_view>

#include <complex>
#include <chrono>
#include <thread>

#include "Integer/Integer.h"

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
	// TEST(List);
	// TEST(ForwardList);
	// TEST(Deque);
	// TEST(Stack);
	// TEST(Heap);
	TEST(Vector);
	// TEST(Valarray);
	// TEST(Map);
	// TEST(MultiMap);
    // TEST(UnorderedMap);
    // TEST(Set);
    // TEST(UnorderedSet);
	// TEST(Queue);
	// TEST(PriorityQueue);
	// TEST(Custom_Vector);
}
