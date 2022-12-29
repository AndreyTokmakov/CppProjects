cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(QT_Projects)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)


# message (STATUS "Qt5Widgets_INCLUDES === ${Qt5Widgets_INCLUDES}")
# message (STATUS "Qt5Widgets_DEFINITIONS === ${Qt5Widgets_DEFINITIONS}")
# message (STATUS "Qt5Widgets_LIBRARIES === ${Qt5Widgets_LIBRARIES}")

# The Qt5Widgets_INCLUDES also includes the include directories for dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})


set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(QT_Projects MACOSX_BUNDLE 
    main.cpp
)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(QT_Projects pthread ${Qt5Widgets_LIBRARIES})
