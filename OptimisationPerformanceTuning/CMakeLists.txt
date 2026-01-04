cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(OptimisationPerformanceTuning)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Inspection/main.cpp
        Utilities.cpp
        Numa/Numa.cpp
        SpecificOptimizationTopics/OptimizationTricks.cpp
        FalseSharing/FalseSharingExperiments.cpp
        Valgrind/ValgrindExperiments.cpp
        CacheLineAccessEffects/CacheLineAccessEffects.cpp
        DoubleBuffering/DoubleBuffering.cpp
        BranchLessCodeExamples/BranchLessCodeExamples.cpp
        Branches/Remove_Branches_Builtin_Unreachable.cpp
        AoS_vs_SoA/AoS_vs_SoA.cpp
        Data_Prefetching/DataPrefetching.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        numa
)