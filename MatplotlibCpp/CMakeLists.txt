cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(MatplotlibCpp)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(${THIRD_PARTY_DIR}/matplotlib-cpp)
include_directories(/usr/include/python3.8)
include_directories(/usr/local/lib/python3.8/dist-packages/numpy/core/include)

# include all components
add_executable(${PROJECT_NAME}
    main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} python3.8)