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

add_executable(${PROJECT_NAME}
    main.cpp
    ObjectPool/ObjectPool.h
    Experiments/Experiments.cpp Experiments/Experiments.h
    Common/Common.cpp Common/Common.h
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        utils
        ${EXTRA_LIBS}
)