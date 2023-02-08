cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Catch2Tests)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(/home/andtokm/DiskS/ProjectsUbuntu/Catch2/src)
include_directories(/home/andtokm/DiskS/ProjectsUbuntu/Catch2/cmake-build-release/generated-includes)

link_directories(/home/andtokm/DiskS/ProjectsUbuntu/Catch2/cmake-build-release/src)

# add_subdirectory(/home/andtokm/DiskS/ProjectsUbuntu/Catch2)

#


# include all components
add_executable(Catch2Tests
        main.cpp
        factorial_tests.cpp test_sections_1.cpp matchers.cpp asserts.cpp)


# Link phread library
TARGET_LINK_LIBRARIES(Catch2Tests pthread libCatch2.a )