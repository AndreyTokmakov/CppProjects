/**============================================================================
Name        : DataStructuresExperiments
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ data structures experiments
============================================================================**/

#include "BitReader.h"
#include "CircularBuffer.h"
#include "DVector.h"
#include "EventLoop.h"
#include "LRUCache.h"
#include "MaxStack.h"
#include "MinStack.h"
#include "MinMaxStack.h"
#include "RateLimiter.h"
#include "RingBuffer.h"
#include "Int2String_MappingTable/Int2String_MappingTable.h"

#include <iostream>

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // BitReader::TestAll();
    // CircularBuffer::TestAll();
    // DVector::TestAll();
    // EventLoop::TestAll();
    // LRUCache::TestAll();
    // MaxStack::TestAll();
    // MinStack::TestAll();
    // MinMaxStack::TestAll();
    // RateLimiter::TestAll();
    // RingBuffer::TestAll();
    Int2String_MappingTable::TestAll();

    return EXIT_SUCCESS;
}
