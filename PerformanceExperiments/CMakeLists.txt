cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(PerformanceExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(PerformanceExperiments 
    main.cpp
    Integer.h Integer.cpp
    MemoryPool.h
    Collections.h Collections.cpp
)

TARGET_LINK_LIBRARIES(PerformanceExperiments)