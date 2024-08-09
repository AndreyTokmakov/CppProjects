/**============================================================================
Name        : main.cpp
Created on  : 09.08.2024
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : DataStructures
============================================================================**/

#include "Array/Array.h"
#include "LRUCache/LRUCache.h"
#include "TTLCache/TTLCache.h"
#include "Function/Function.h"
#include "ExecutorAdapter/ExecutorAdapter.h"
#include "ExecutorAdapter2/ExecutorAdapter2.h"
#include "HashTable/HashTable.h"
#include "Finalizers/Finalizers.h"
#include "BinaryTree/BinaryTree.h"
#include "BitSet/BitSet.h"
#include "CircularBuffers/CircularBuffers.h"
#include "ThreadsafeQueue/ThreadsafeQueue.h"
#include "SharedPtr/SharedPtr.h"
#include "ObjectPools/ObjectPools.h"
#include "Optional/Optional.h"
#include "Logger/Logger.h"
#include "MinHeap/MinHeap.h"
#include "MinStack/MinStack.h"
#include "MinMaxStack/MinMaxStack.h"
#include "Memory/AlignedStackAllocator.h"
#include "Memory/CustomStackAllocator.h"
#include "Heap_WithComparator/HeapWithComparator.h"
#include "ReferenceWrapper/ReferenceWrapper.h"
#include "CustomVector/CustomVector.h"
#include "LinkedList/LinkedList.h"
#include "ThreadPoolOne/ThreadPoolOne.h"
#include "ThreadPoolTwo/ThreadPoolTwo.h"
#include "ThreadPoolLimited/ThreadPoolLimited.h"
#include "TwoSidedVector/TwoSidedVector.h"
#include "CategoryTree/CategoryTree.h"
#include "PriorityList/PriorityList.h"
#include "LockFreeDataStructures/LockFreeQueue.h"
#include "RAII_CommitWrapper/RAII_CommitWrapper.h"
#include "ObjectNumberLimiter/ObjectNumberLimiter.h"
#include "Compressed_Pair/CompressedPair.h"
#include "Trie/Trie.h"


// TODO:
// 1. Add unit-test for LRUCache


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    /// Unimplemented
    // BinaryTree::TestAll();


    // Array::TEST_ALL();
    // BitSet::TEST_ALL();
    // CategoryTree::TEST_ALL();
    // CircularBuffers::TEST_ALL();
    // CustomVector::TestAll();
    // ExecutorAdapter::TestAll();
    // ExecutorAdapter2::TestAll();
    // Finalizers::TestAll();
    // Function::Test();
    // HashTable::TestAll();
    // LinkedLists::TEST_ALL();
    // Logging::TEST_ALL();
	// LRUCache::TEST_ALL();
    // ObjectPools::TEST_ALL();
    // Optional::TEST_ALL();
    // PriorityList::TEST_ALL();
    // RAII_CommitWrapper::TEST_ALL();
    // ReferenceWrapper::Test();
    // TwoSidedVector::TestAll();
    // MinStack::TestAll();
    // MinMaxStack::TestAll();
    // MinHeap::TestAll();
    // HeapWithComparator::TestAll();
    CompressedPair::TestAll();

    // Memory::AlignedStackAllocator::TestAll();
    // Memory::CustomStackAllocator::TestAll();

    // Memory::Shared_Ptr_Tests();

    // ThreadPoolOne::TEST_ALL();
    // ThreadPoolTwo::TEST_ALL();
    // ThreadPoolLimited::TestAll();

    // Trie::TestAll();

    // ObjectNumberLimiter::TestAll();

    // Queues::Multithreading::RunTests();

	// TTLCache::TEST_ALL();

    // LockFreeQueue::TEST_ALL();
}
