cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(EigenTests)
add_compile_options(-c -Wall -Werror -Wextra -O3 -std=c++2a)

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
SET(PROJECT_VERSION 1.0.0.0)

include_directories(../../third_party/eigen/Eigen)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
    main.cpp
    LeastSquares/LeastSquares.cpp LeastSquares/LeastSquares.h
    Matrices/Matrices.cpp Matrices/Matrices.h
    Vectors/Vectors.cpp Vectors/Vectors.h
        Quaternion/Quaternion.cpp Quaternion/Quaternion.h Arrays/Arrays.cpp Arrays/Arrays.h)