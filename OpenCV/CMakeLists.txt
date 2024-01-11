cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(OpenCV)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# SET(OPENSSL_ROOT_DIR "/home/andtokm/DiskS/ProjectsUbuntu/third_party/openssl")
# link_directories(${OPENSSL_ROOT_DIR})
# include_directories("${OPENSSL_ROOT_DIR}/include")
# message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")

find_package(OpenCV REQUIRED)
# message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
    main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})