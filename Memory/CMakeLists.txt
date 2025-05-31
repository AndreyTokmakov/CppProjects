cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Memory)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

include_directories(${THIRD_PARTY_DIR}/tcmalloc)
include_directories(${THIRD_PARTY_DIR}/abseil-cpp)
# link_directories(${THIRD_PARTY_DIR}/tcmalloc/)

set(PROJECT_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
set(COMMON ${PROJECT_DIR}/common)
set(EXPERIMENTS ${PROJECT_DIR}/experiments)
set(OBJECT_POOL ${PROJECT_DIR}/object_pool)

include_directories(${COMMON})
include_directories(${EXPERIMENTS})
include_directories(${OBJECT_POOL})


add_executable(${PROJECT_NAME}
        main.cpp
        ${OBJECT_POOL}/ObjectPool.h
        ${EXPERIMENTS}/Experiments.cpp ${EXPERIMENTS}/Experiments.h
        ${COMMON}/Common.cpp ${COMMON}/Common.h
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        utils
        ${EXTRA_LIBS}
)