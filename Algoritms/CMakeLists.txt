cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Algoritms)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(Algoritms
    	main.cpp
		Graphs/Graphs.cpp Graphs/Graphs.h
		Heap/Heap.cpp Heap/Heap.h
		Lists/Lists.cpp Lists/Lists.h
		Interviews/Interviews.cpp Interviews/Interviews.h
		Queues/Queues.cpp Queues/Queues.h
		Sorting/Sorting.cpp Sorting/Sorting.h
		Stack/Stack.cpp Stack/Stack.h
		Strings/Strings.cpp Strings/Strings.h
		Trees/Trees.cpp Trees/Trees.h
		Numeric/Numeric.cpp Numeric/Numeric.h
		Utilities/Utilities.cpp Utilities/Utilities.h
        MultiThreading/MultiThreading.cpp MultiThreading/MultiThreading.h
)

TARGET_LINK_LIBRARIES(Algoritms pthread)