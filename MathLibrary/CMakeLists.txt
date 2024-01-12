cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(MathLibrary)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    main.cpp
    LinearRegression.cpp LinearRegression.h
	Types/Types.cpp Types/Types.h
	TypesEx/TypesEx.cpp TypesEx/TypesEx.h
	Quaternions/Quaternion.cpp Quaternions/Quaternion.h
    IntegralCalculations/LeftRectanglesMethod.cpp
    IntegralCalculations/IntegralCalculations.h
	Utilities.h
)

 # Link phread library
TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)