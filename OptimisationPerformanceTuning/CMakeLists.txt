cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(OptimisationPerformanceTuning)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Inspection/CodeInspection.cpp
        Utilities.h Utilities.cpp
        Numa/Numa.h Numa/Numa.cpp
        SpecificOptimizationTopics/OptimizationTricks.cpp SpecificOptimizationTopics/OptimizationTricks.h
        FalseSharing/FalseSharingExperiments.cpp FalseSharing/FalseSharingExperiments.h
        Valgrind/ValgrindExperiments.cpp Valgrind/ValgrindExperiments.h
        CacheLineAccessEffects/CacheLineAccessEffects.cpp CacheLineAccessEffects/CacheLineAccessEffects.h
        DoubleBuffering/DoubleBuffering.cpp DoubleBuffering/DoubleBuffering.h
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        numa
)