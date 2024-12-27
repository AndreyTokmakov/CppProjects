cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DataStructures)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
		main.cpp
		BitSet/BitSet.cpp
		BitFlags/BitFlags.cpp
		CircularBuffers/CircularBuffers.cpp
		Logger/Logger.cpp
		LRUCache/LRUCache.cpp
		ObjectPools/ObjectPools.cpp
		LockFreeDataStructures/LockFreeQueue.cpp
		Finalizers/Finalizers.cpp
		Optional/Optional.cpp
		SharedPtr/SharedPtr.cpp
		ThreadsafeQueue/ThreadsafeQueue.cpp
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
		Trie/Trie.cpp
		Utilities/Long.cpp
        TwoSidedVector/TwoSidedVector.cpp
		MinMaxStack/MinMaxStack.cpp
		MinStack/MinStack.cpp
		MinHeap/MinHeap.cpp
		Heap_WithComparator/HeapWithComparator.cpp
		Compressed_Pair/CompressedPair.cpp
		Wrapper/Wrapper.h
		ThreadPool/ThreadPool.cpp
		RingBuffer/RingBuffer.cpp
		RingBuffer/RingBuffer_SPSC.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
)