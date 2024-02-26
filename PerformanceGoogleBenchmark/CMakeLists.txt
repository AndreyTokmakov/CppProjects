cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(PerformanceGoogleBenchmark)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(../../third_party/benchmark/include)
link_directories(../../third_party/benchmark/build/lib)
link_directories(../../third_party/benchmark/build/src)

find_package(benchmark REQUIRED)

# include all components
add_executable(${PROJECT_NAME}
    main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        tbb -lbenchmark -lpthread
)
