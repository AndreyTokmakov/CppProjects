cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DataStructures)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(DataStructures MACOSX_BUNDLE 
    main.cpp
	BitSet/BitSet.cpp BitSet/BitSet.h
	CircularBuffers/CircularBuffers.cpp CircularBuffers/CircularBuffers.h
	Logger/Logger.cpp Logger/Logger.h
	LRUCache/LRUCache.cpp LRUCache/LRUCache.h
	ObjectPools/ObjectPools.cpp ObjectPools/ObjectPools.h
	LockFreeDataStructures/LockFreeQueue.cpp LockFreeDataStructures/LockFreeQueue.h
	Optional/Optional.cpp Optional/Optional.h
	SharedPtr/SharedPtr.cpp SharedPtr/SharedPtr.h
	ThreadsafeQueue/ThreadsafeQueue.cpp ThreadsafeQueue/ThreadsafeQueue.h
	TTLCache/TTLCache.cpp TTLCache/TTLCache.h
	Utilities/Integer.cpp Utilities/Integer.h
	Utilities/ThreadHelperUtilities.cpp Utilities/ThreadHelperUtilities.h
	CustomVector/CustomVector.cpp CustomVector/CustomVector.h
	ReferenceWrapper/ReferenceWrapper.cpp ReferenceWrapper/ReferenceWrapper.h
	LinkedList/LinkedList.cpp LinkedList/LinkedList.h
	ThreadPoolOne/ThreadPoolOne.cpp ThreadPoolOne/ThreadPoolOne.h
	ThreadPoolTwo/ThreadPoolTwo.cpp ThreadPoolTwo/ThreadPoolTwo.h
	CategoryTree/CategoryTree.cpp CategoryTree/CategoryTree.h
    PriorityList/PriorityList.cpp PriorityList/PriorityList.h
        RAII_CommitWrapper/RAII_CommitWrapper.cpp RAII_CommitWrapper/RAII_CommitWrapper.h Array/Array.cpp Array/Array.h Function/Function.cpp Function/Function.h ExecutorAdapter/ExecutorAdapter.cpp ExecutorAdapter/ExecutorAdapter.h HashTable/HashTable.cpp HashTable/HashTable.h ExecutorAdapter2/ExecutorAdapter2.cpp ExecutorAdapter2/ExecutorAdapter2.h ObjectNumberLimiter/ObjectNumberLimiter.cpp ObjectNumberLimiter/ObjectNumberLimiter.h BinaryTree/BinaryTree.cpp BinaryTree/BinaryTree.h ThreadPoolLimited/ThreadPoolLimited.cpp ThreadPoolLimited/ThreadPoolLimited.h Trie/Trie.cpp Trie/Trie.h Utilities/Long.cpp Utilities/Long.h
        TwoSidedVector/TwoSidedVector.cpp
        TwoSidedVector/TwoSidedVector.h
		MinStack/MinStack.cpp
		MinStack/MinStack.h
		MinHeap/MinHeap.cpp
		MinHeap/MinHeap.h)

 # Link phread library
TARGET_LINK_LIBRARIES(DataStructures pthread)