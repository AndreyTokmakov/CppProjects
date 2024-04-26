cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Logging)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

include_directories(../../third_party/quill/quill/include)
link_directories(../../third_party/quill/build/quill)

# find_package(quill REQUIRED)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE
        pthread
        tbb
        libquill.a
        # quill::quill
)