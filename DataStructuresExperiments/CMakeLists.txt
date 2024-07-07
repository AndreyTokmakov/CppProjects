cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DataStructuresExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(BitReader)
include_directories(CircularBuffer)
include_directories(DVector)
include_directories(EventLoop)
include_directories(LRUCache)
include_directories(MaxStack)
include_directories(MinMaxStack)
include_directories(MinStack)
include_directories(RingBuffer)
include_directories(RateLimiter)
include_directories(Utils)

# include all components
add_executable(${PROJECT_NAME}
		main.cpp
		CircularBuffer/CircularBuffer.h CircularBuffer/CircularBuffer.cpp
		BitReader/BitReader.h BitReader/BitReader.cpp
		DVector/DVector.h DVector/DVector.cpp
		EventLoop/EventLoop.h EventLoop/EventLoop.cpp
		LRUCache/LRUCache.h LRUCache/LRUCache.cpp
		MaxStack/MaxStack.h MaxStack/MaxStack.cpp
		MinMaxStack/MinMaxStack.h MinMaxStack/MinMaxStack.cpp
		MinStack/MinStack.h MinStack/MinStack.cpp
		RingBuffer/RingBuffer.h RingBuffer/RingBuffer.cpp
		RateLimiter/RateLimiter.h RateLimiter/RateLimiter.cpp
		Utils/Wrapper.h
		Utils/PertTools.h Utils/PertTools.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
)