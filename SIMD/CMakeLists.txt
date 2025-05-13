cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)

project(SIMD)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26
        -fno-rtti
        -fno-exceptions
        -mfma
        -mavx
        -mavx2
        -mavx512f
)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
)

# include all components
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
)