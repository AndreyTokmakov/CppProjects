cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)

project(Collections)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
        Array/Array.cpp
        BinaryTrees/BinaryTrees.cpp
        Custom_Vector/Custom_Vector.cpp
        Deque/Deque.cpp
        ForwardList/ForwardList.cpp
        Heap/Heap.cpp
        List/List.cpp
        Map/Map.cpp
        PriorityQueue/PriorityQueue.cpp
        Queue/Queue.cpp
        Set/Set.cpp
        Stack/Stack.cpp
        UnorderedMap/UnorderedMap.cpp
        UnorderedSet/UnorderedSet.cpp
        Valarray/Valarray.cpp
        Vector/Vector.cpp
        MultiMap/MultiMap.cpp
        Helpers/Integer.cpp
        Flat_Map/FlatMap.cpp
)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        utils
        pthread
)