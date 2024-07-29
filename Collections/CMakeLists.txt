cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Collections)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
        Array/Array.cpp Array/Array.h
        BinaryTrees/BinaryTrees.cpp BinaryTrees/BinaryTrees.h
        Custom_Vector/Custom_Vector.cpp Custom_Vector/Custom_Vector.h
        Deque/Deque.cpp Deque/Deque.h
        ForwardList/ForwardList.cpp ForwardList/ForwardList.h
        Heap/Heap.cpp Heap/Heap.h

        List/List.cpp List/List.h
        Map/Map.cpp Map/Map.h
        PriorityQueue/PriorityQueue.cpp PriorityQueue/PriorityQueue.h
        Queue/Queue.cpp Queue/Queue.h
        Set/Set.cpp Set/Set.h
        Stack/Stack.cpp Stack/Stack.h
        UnorderedMap/UnorderedMap.cpp UnorderedMap/UnorderedMap.h
        UnorderedSet/UnorderedSet.cpp UnorderedSet/UnorderedSet.h
        Valarray/Valarray.cpp Valarray/Valarray.h
        Vector/Vector.cpp Vector/Vector.h
        MultiMap/MultiMap.cpp MultiMap/MultiMap.h

        Helpers/Integer.cpp Helpers/Integer.h
        Helpers/Helpers.h Helpers/Wrapper.h
 )

# include all components
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
)