cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(MachineLearningExperiments)

set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)
include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/eigen/Eigen)
include_directories(/home/andtokm/Projects/third_party/matplotlib-cpp)

# include_directories(/usr/include/python3.8)
# include_directories(/usr/local/lib/python3.8/dist-packages/numpy/core/include)


if(Boost_VERSION LESS 106500)
	message(STATUS "Boost version: Boost_VERSION=${Boost_VERSION}")
	message(FATAL "Required Boost version >= 1.65.0")
endif()
message (STATUS "BOOST VERSION: ${Boost_VERSION}")

find_package(Boost REQUIRED COMPONENTS
		system filesystem serialization
)

SET(CMAKE_CXX_FLAGS -fopenmp)

# include all components
add_executable(${PROJECT_NAME}  MACOSX_BUNDLE
    main.cpp
	Utilities/Utilities.h Utilities/Utilities.cpp
	Utilities/Activation.cpp Utilities/Activation.h
	LinearRegression/LinearRegression.h LinearRegression/LinearRegression.cpp
	CSVReader/Csv.cpp CSVReader/Csv.h
	Types/Types.h Types/Types.cpp
	Experiments/Experiments.h Experiments/Experiments.cpp
	Eigen/EigenLinearRegression.h Eigen/EigenLinearRegression.cpp
    MLPack/MLPack.cpp MLPack/MLPack.h
	Eigen/PolynomialFitting.cpp Eigen/PolynomialFitting.h 
	LogisticRegression/LogisticRegression.cpp LogisticRegression/LogisticRegression.h
)

 # Link phread library
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		# pthread mlpack blas armadillo lapack ${Boost_LIBRARIES} python3.8
)