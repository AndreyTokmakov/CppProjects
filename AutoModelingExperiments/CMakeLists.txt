cmake_minimum_required(VERSION 3.20)
project(AutoModelingExperiments)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")

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
    message("Skipping AutoModelingExperiments: ${VTK_NOT_FOUND_MESSAGE}")
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

add_executable(AutoModelingExperiments
        main.cpp
        VtkHeaders.h
        Types.h
        Features.cpp
        Features.h 
        Utilities.cpp 
        Utilities.h
        DiagnosticsMethods.cpp 
        DiagnosticsMethods.h 
        Experiments.cpp 
        Experiments.h 
        Axis.cpp 
        Axis.h 
        ClassifyTeeth.cpp 
        ClassifyTeeth.h
        ContactPoints/ContactPoints.cpp
        ContactPoints/ContactPoints.h
        Types.cpp
        ModelOrientation.cpp
        ModelOrientation.h
        MachineLearningExperiments.cpp
        MachineLearningExperiments.h
        TreatmentPlan/TreatmentPlan.h
        TreatmentPlan/TreatmentPlan.cpp
        TreatmentPlan/TreatmentPlan_Tests.cpp
        TreatmentPlan/TreatmentPlan_UnitTests.cpp
        Preprocessing.cpp Preprocessing.h)

target_link_libraries(AutoModelingExperiments PRIVATE ${VTK_LIBRARIES})
vtk_module_autoinit(TARGETS AutoModelingExperiments MODULES ${VTK_LIBRARIES})