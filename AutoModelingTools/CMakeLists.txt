cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(AutoModelingTools)
add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
SET(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if(Boost_VERSION LESS 106500)
    message(STATUS "Boost version: Boost_VERSION=${Boost_VERSION}")
    message(FATAL "Required Boost version >= 1.65.0")
endif()
message (STATUS "BOOST VERSION: ${Boost_VERSION}")

find_package(Boost REQUIRED COMPONENTS 
    log_setup 
    log
    filesystem 
    program_options
)


# include all components
add_executable(AutoModelingTools MACOSX_BUNDLE 
    main.cpp
    TreatmentPlan/TreatmentPlan.h
    TreatmentPlan/TreatmentPlan.cpp
    TreatmentPlan/TreatmentPlan_Tests.cpp
    TreatmentPlan/TreatmentPlan_UnitTests.cpp
    Types.h
    Types.cpp
    Utilities.h
    Utilities.cpp
)

 # Link phread library
TARGET_LINK_LIBRARIES(AutoModelingTools pthread)