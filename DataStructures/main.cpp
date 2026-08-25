/**============================================================================
Name        : main.cpp
Created on  : 09.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataStructures
============================================================================**/

#include "Array/Array.h"
#include "Any/Any.hpp"
#include "BitFlags/BitFlags.h"
#include "Barrier/Barrier.hpp"
#include "Semaphore/Semaphore.hpp"
#include "LRUCache/LRUCache.h"
#include "TTLCache/TTLCache.h"
#include "Function/Function.hpp"
#include "ExecutorAdapter/ExecutorAdapter.h"
#include "ExecutorAdapter2/ExecutorAdapter2.h"
#include "HashTable/HashTable.h"
#include "Finalizers/Finalizers.h"
#include "BinaryTree/BinaryTree.h"
#include "BitSet/BitSet.h"
#include "FlatMap/FlatMap.hpp"
#include "FlatMap_Fast/FlatMap.hpp"
#include "FreeList/FreeList.hpp"
#include "FreeList_ThreadSafe/FreeList_ThreadSafe.hpp"
#include "CircularBuffers/CircularBuffers.h"
#include "ThreadsafeQueue/ThreadsafeQueue.h"
#include "ThreadSafe_BoundedQueue/ThreadSafe_BoundedQueue.hpp"
#include "SharedPtr/SharedPtr.h"
#include "ObjectPool/ObjectPool.h"
#include "ObjectPool_Growing/ObjectPool_Growing.h"
#include "GenericTableTemplate/GenericTableTemplate.h"
#include "Optional/Optional.h"
#include "Expected/Expected.h"
#include "Logger/Logger.h"
#include "RingBuffer/RingBuffer.h"
#include "RingBuffer/RingBufferEx.h"
#include "RingBuffer/RingBufferExAtomic.h"
#include "RingBuffer/RingBuffer_SPSC.h"
#include "RingBufferFast/RingBufferFast.hpp"
#include "MinHeap/MinHeap.h"
#include "Invoker_MethodClass/Invoker.h"
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
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/ThreadPoolEx.h"
#include "Trie/Trie.h"
#include "TypeListChecker/TypeListChecker.hpp"
#include "UniquePtr/UniquePtr.hpp"
#include "StaticSortedSearchArray/StaticSortedSearchArray.hpp"
#include "StaticString/StaticString.hpp"
#include "MPMCQueue/MPMCQueue.hpp"
#include "MineSet_Fast/MiniSet.hpp"
#include "StateManager/StateManager.hpp"
#include "SynchronizedWrapper/SynchronizedWrapper.hpp"
#include "TaskExecutor/Executors.hpp"
#include "TaskSBO/TaskSBO.hpp"
#include "Published_Subscriber/PubSub.hpp"
#include "Queues/Queues.hpp"
#include "MeticsCollector/metrics.hpp"
#include "CommandWrapper/CommandWrapper.hpp"



// TODO:
// 1. Add unit-test for LRUCache


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    /// Unimplemented
    // BinaryTree::TestAll();

    // any::TestAll();
    // Array::TEST_ALL();
    // BitSet::TEST_ALL();
    // BitFlags::TestAll();
    // CategoryTree::TEST_ALL();
    // CustomVector::TestAll();
    // ExecutorAdapter::TestAll();
    // ExecutorAdapter2::TestAll();
    // Finalizers::TestAll();
    // FlatMap::TestAll();
    // flat_map_fast::TestAll();
    // Function::Test();
    // HashTable::TestAll();
    // LinkedLists::TEST_ALL();
    // Logging::TEST_ALL();
	// LRUCache::TEST_ALL();
    // ObjectPool::TestAll();
    // ObjectPool_Growing::TestAll();
    // Optional::TEST_ALL();
    // Expected::TestAll();
    // PriorityList::TEST_ALL();
    // RAII_CommitWrapper::Test();
    // ReferenceWrapper::Test();
    // TwoSidedVector::TestAll();
    // MinStack::TestAll();
    // MinMaxStack::TestAll();
    // MinHeap::TestAll();
    // Invoker::TestAll();
    // HeapWithComparator::TestAll();
    // CompressedPair::TestAll();

	// free_list::TestAll();
	// free_list_thread_safe::TestAll();

	// static_sorted_search_array::TestAll();
    // CircularBuffers::TEST_ALL();
    // RingBuffer::TestAll();
    // RingBufferEx::TestAll();
    // RingBufferExAtomic::TestAll();
    // RingBuffer_SPSC::TestAll();
	ring_buffer_fast::TestAll();

	// static_string::TestAll();

	// MPMCQueue::TestAll();

	// queues::TestAll();

	// metrics::TestAll();

	// scommand_wrapper::TestAll();

	// published_subscriber::TestAll();

    // GenericTableTemplate::TestAll();

    // Memory::AlignedStackAllocator::TestAll();
    // Memory::CustomStackAllocator::TestAll();

	// TypeListChecker::TestAll();

    // Memory::Shared_Ptr_Tests();
    // Memory::UniquePtrTests();

    // Trie::TestAll();

    // ObjectNumberLimiter::TestAll();

	// TTLCache::TEST_ALL();

	// mini_set::TestAll();

	// state_manager::TestAll();

	// synchronized_wrapper::TestAll();

	// task_executor::TestAll();
	// tast_sbo::TestAll();

	/*********************************** Multithreading ***********************************/

    // ThreadPool::TestAll();
    // ThreadPoolEx::TestAll();
    // ThreadPoolOne::TEST_ALL();
    // ThreadPoolTwo::TEST_ALL();
    // ThreadPoolLimited::TestAll();

	// thread_safe_bounded_queue::TestAll();

    // Queues::Multithreading::RunTests();

	// LockFreeQueue::TEST_ALL();

    // barrier::TestAll();

    // semaphore::TestAll();
}
