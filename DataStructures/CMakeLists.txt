cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DataStructures)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
		main.cpp
		BitSet/BitSet.cpp BitSet/BitSet.h
		CircularBuffers/CircularBuffers.cpp CircularBuffers/CircularBuffers.h
		Logger/Logger.cpp Logger/Logger.h
		LRUCache/LRUCache.cpp LRUCache/LRUCache.h
		ObjectPools/ObjectPools.cpp ObjectPools/ObjectPools.h
		LockFreeDataStructures/LockFreeQueue.cpp LockFreeDataStructures/LockFreeQueue.h
		Finalizers/Finalizers.cpp Finalizers/Finalizers.h
		Optional/Optional.cpp Optional/Optional.h
		SharedPtr/SharedPtr.cpp SharedPtr/SharedPtr.h
		ThreadsafeQueue/ThreadsafeQueue.cpp ThreadsafeQueue/ThreadsafeQueue.h
		TTLCache/TTLCache.cpp TTLCache/TTLCache.h
		Utilities/Integer.cpp Utilities/Integer.h
		Utilities/ThreadHelperUtilities.cpp Utilities/ThreadHelperUtilities.h
		Memory/AlignedStackAllocator.cpp Memory/AlignedStackAllocator.h
		Memory/CustomStackAllocator.cpp Memory/CustomStackAllocator.h
		CustomVector/CustomVector.cpp CustomVector/CustomVector.h
		ReferenceWrapper/ReferenceWrapper.cpp ReferenceWrapper/ReferenceWrapper.h
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
		ExecutorAdapter2/ExecutorAdapter2.cpp ExecutorAdapter2/ExecutorAdapter2.h
		ObjectNumberLimiter/ObjectNumberLimiter.cpp ObjectNumberLimiter/ObjectNumberLimiter.h
		BinaryTree/BinaryTree.cpp BinaryTree/BinaryTree.h
		ThreadPoolLimited/ThreadPoolLimited.cpp ThreadPoolLimited/ThreadPoolLimited.h
		Trie/Trie.cpp Trie/Trie.h
		Utilities/Long.cpp Utilities/Long.h
        TwoSidedVector/TwoSidedVector.cpp TwoSidedVector/TwoSidedVector.h
		MinMaxStack/MinMaxStack.cpp
		MinStack/MinStack.cpp MinStack/MinStack.h
		MinHeap/MinHeap.cpp MinHeap/MinHeap.h
		Heap_WithComparator/HeapWithComparator.cpp Heap_WithComparator/HeapWithComparator.h
		Compressed_Pair/CompressedPair.cpp  Compressed_Pair/CompressedPair.h
		Wrapper/Wrapper.h
		ThreadPool/ThreadPool.cpp
		ThreadPool/ThreadPool.h
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
)