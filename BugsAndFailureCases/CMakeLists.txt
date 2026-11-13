cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(BugsAndFailureCases)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        utils
        ${EXTRA_LIBS}
)