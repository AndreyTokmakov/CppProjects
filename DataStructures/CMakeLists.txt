cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DataStructures)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

# include all components
add_executable(${PROJECT_NAME}
		main.cpp
		Any/Any.cpp
		BitSet/BitSet.cpp
		BitFlags/BitFlags.cpp
		CircularBuffers/CircularBuffers.cpp
		CommandWrapper/CommandWrapper.cpp
		Logger/Logger.cpp
		LRUCache/LRUCache.cpp
        ObjectPool/ObjectPool.cpp
		ObjectPool_Growing/ObjectPool_Growing.cpp
		LockFreeDataStructures/LockFreeQueue.cpp
		FreeList/FreeList.cpp
		FreeList_ThreadSafe/FreeList_ThreadSafe.cpp
		Finalizers/Finalizers.cpp
		FlatMap/FlatMap.cpp
		FlatMap_Fast/FlatMap.cpp
		Optional/Optional.cpp
		Expected/Expected.cpp
		ThreadsafeQueue/ThreadsafeQueue.cpp
		ThreadSafe_BoundedQueue/ThreadSafe_BoundedQueue.cpp
		TTLCache/TTLCache.cpp
		Utilities/Integer.cpp
		Utilities/ThreadHelperUtilities.cpp
		Memory/AlignedStackAllocator.cpp
		Memory/CustomStackAllocator.cpp
		CustomVector/CustomVector.cpp
		ReferenceWrapper/ReferenceWrapper.cpp
		LinkedList/LinkedList.cpp
		ThreadPoolOne/ThreadPoolOne.cpp
		ThreadPoolTwo/ThreadPoolTwo.cpp
		CategoryTree/CategoryTree.cpp
		PriorityList/PriorityList.cpp
        RAII_CommitWrapper/RAII_CommitWrapper.cpp
		Array/Array.cpp
		GenericTableTemplate/GenericTableTemplate.cpp
		Function/Function.cpp
		ExecutorAdapter/ExecutorAdapter.cpp
		HashTable/HashTable.cpp
		ExecutorAdapter2/ExecutorAdapter2.cpp
		ObjectNumberLimiter/ObjectNumberLimiter.cpp
		BinaryTree/BinaryTree.cpp
		ThreadPoolLimited/ThreadPoolLimited.cpp
		StaticSortedSearchArray/StaticSortedSearchArray.cpp
		Trie/Trie.cpp
		TypeListChecker/TypeListChecker.cpp
		Utilities/Long.cpp
        TwoSidedVector/TwoSidedVector.cpp
		MinMaxStack/MinMaxStack.cpp
		MinStack/MinStack.cpp
		MPMCQueue/MPMCQueue.cpp
		MinHeap/MinHeap.cpp
		Heap_WithComparator/HeapWithComparator.cpp
		Invoker_MethodClass/Invoker.cpp
		Compressed_Pair/CompressedPair.cpp
		Wrapper/Wrapper.h
		ThreadPool/ThreadPool.cpp
		ThreadPool/ThreadPoolEx.cpp
		RingBufferFast/RingBufferFast.cpp
		RingBuffer/RingBuffer.cpp
		RingBuffer/RingBufferEx.cpp
		RingBuffer/RingBufferExAtomic.cpp
		RingBuffer/RingBuffer_SPSC.cpp
		StaticString/StaticString.cpp

		Queues/Queues.cpp
		Queues/PriorityDispatcher.cpp
		Queues/MultiLevelQueue.cpp

		SynchronizedWrapper/SynchronizedWrapper.cpp

		SharedPtr/SharedPtr.cpp
		UniquePtr/UniquePtr.cpp

		Published_Subscriber/PubSub.cpp
		Published_Subscriber/SimpleSubscriberCallback.cpp
		Published_Subscriber/SubscriberDifferentEvents.cpp
		Published_Subscriber/SubscriberDifferentTopicsStatic.cpp
		Published_Subscriber/SubscriberDifferentTopicsDeferRemove.cpp

		Barrier/Barrier.cpp
		Semaphore/Semaphore.cpp

		TaskExecutor/Executors.cpp
		TaskExecutor/TaskExecutor1.cpp
		TaskExecutor/TaskExecutor2.cpp

		TaskSBO/TaskSBO.cpp

		MineSet_Fast/MiniSet.cpp
		StateManager/StateManager.cpp

		MeticsCollector/metrics.cpp
		MeticsCollector/MetricsController.cpp
		MeticsCollector/MetricsControllerEx.cpp


)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")

target_link_libraries(${PROJECT_NAME} PUBLIC
		utils
		pthread
)