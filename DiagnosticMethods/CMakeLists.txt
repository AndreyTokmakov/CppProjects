cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DiagnosticMethods)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/Eigen/Eigen)

find_package(VTK COMPONENTS
        vtkCommonColor
        vtkCommonCore
        vtkCommonDataModel
        vtkFiltersCore
        vtkIOParallel
        vtkInteractionStyle
        #   vtkRenderingContextOpenGL2
        vtkRenderingCore
        vtkRenderingFreeType
        #   vtkRenderingGL2PSOpenGL2
        vtkRenderingLOD
        vtkRenderingOpenGL2
        vtkFiltersSources
        vtkIOGeometry
        vtkIOLegacy
        vtkIOPLY
        vtkIOXML
        vtkIOImage
        vtkImagingSources
        vtkFiltersExtraction
        vtkCommonMath
        vtkFiltersFlowPaths
        vtkFiltersModeling
        vtkInteractionWidgets
        vtkFiltersGeneral
        vtkFiltersGeometry
        vtkCommonTransforms
        vtkRenderingAnnotation
        vtkImagingCore
        vtkRenderingVolume
        vtkRenderingVolumeOpenGL2
        vtkImagingHybrid
        QUIET
        )

if (NOT VTK_FOUND)
    message("Skipping VTK_Experiments: ${VTK_NOT_FOUND_MESSAGE}")
    return ()
endif()
message (STATUS "VTK version: ${VTK_VERSION}")
message (STATUS "VTK dir: ${VTK_DIR}")

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


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a )

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    main.cpp
    VtkHeaders.h
    Utilities/Utilities.cpp Utilities/Utilities.h
    TreatmentPlan/TreatmentPlan.cpp
    TreatmentPlan/TreatmentPlan.h TreatmentPlan/TreatmentPlan_Tests.cpp
    TreatmentPlan/TreatmentPlan_UnitTests.cpp
        Model/Tooth.cpp Model/Tooth.h Model/Jaw.cpp Model/Jaw.h Model/ToothAxis.cpp Model/ToothAxis.h Model/Common.h Model/FDI.cpp Model/FDI.h Methods/SpeeСurve.cpp Methods/SpeeСurve.h ../../../../../../Base64.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES} pthread)
vtk_module_autoinit(TARGETS DiagnosticMethods MODULES ${VTK_LIBRARIES})