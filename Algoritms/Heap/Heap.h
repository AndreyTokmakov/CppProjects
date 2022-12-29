//============================================================================
// Name        : Heap.h
// Created on  : 29.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Heap src class
//============================================================================

#ifndef HEAP_INCLUDE_GUARD__H
#define HEAP_INCLUDE_GUARD__H

#include <iostream>
#include "Heap.h"

template<typename ...Args>
void DEBUG(Args&&... args) {
#ifdef DEBUG_OUTPUT
	(std::cout << ... << std::forward<Args>(args)) << std::endl;
#endif
}

namespace Heap_Algoritms {
	void TEST_ALL();
};

#endif // !HEAP_INCLUDE_GUARD__H

