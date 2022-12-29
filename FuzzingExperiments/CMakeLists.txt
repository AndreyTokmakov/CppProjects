cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(FuzzingExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(FuzzingExperiments 
    main.cpp
)

TARGET_LINK_LIBRARIES(FuzzingExperiments pthread)