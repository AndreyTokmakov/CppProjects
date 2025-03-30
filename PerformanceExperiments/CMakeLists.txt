cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(PerformanceExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Integer.h Integer.cpp
        MemoryPool.h
        Collections/Collections.h Collections/Collections.cpp
        VirtualCall_vs_TypeErasure/VirtualCall_vs_TypeErasure.h VirtualCall_vs_TypeErasure/VirtualCall_vs_TypeErasure.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        ${EXTRA_LIBS}
)
