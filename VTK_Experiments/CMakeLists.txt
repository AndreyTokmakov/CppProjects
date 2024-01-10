cmake_minimum_required(VERSION 3.20)
project(VTK_Experiments)

# set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")

include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/Eigen/Eigen)
include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/VTK/Common/Color)
include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/VTK/build/Common/Color)

find_package(VTK
        # vtkCommonColor
        # vtkCommonCore
        # vtkCommonDataModel
        # vtkFiltersCore
        # vtkIOParallel
        # vtkInteractionStyle
        # vtkRenderingContextOpenGL2
        # vtkRenderingCore
        # vtkRenderingFreeType
        # vtkRenderingGL2PSOpenGL2
        # vtkRenderingLOD
        # vtkRenderingOpenGL2
        # vtkFiltersSources
        # vtkIOGeometry
        # vtkIOLegacy
        # vtkIOPLY
        # vtkIOXML
        # vtkIOImage
        # vtkImagingSources
        # vtkFiltersExtraction
        # vtkCommonMath
        # vtkFiltersFlowPaths
        # vtkFiltersModeling
        # vtkInteractionWidgets
        # vtkFiltersGeneral
        # vtkFiltersGeometry
        # vtkCommonTransforms
        # vtkRenderingAnnotation
        # vtkImagingCore
        # vtkRenderingVolume
        # vtkRenderingVolumeOpenGL2
        # vtkImagingHybrid
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

add_executable(VTK_Experiments
        main.cpp
        VtkHeaders.h
        Utilities/Utilities.cpp Utilities/Utilities.h
        Types.cpp Types.h
        Intersection/IntersectionTests.cpp Intersection/IntersectionTests.h
        VtkMatrices/VtkMatrices.cpp VtkMatrices/VtkMatrices.h
        Curvatures/Curvatures.cpp Curvatures/Curvatures.h
        Text/Text.cpp Text/Text.h
        OBBTreeTests/OBBTreeTests.cpp OBBTreeTests/OBBTreeTests.h
        BoundsAndBorders/BoundsAndBorders.cpp BoundsAndBorders/BoundsAndBorders.h
        CovnexHull/CovnexHull.cpp CovnexHull/CovnexHull.h
        Holes/Holes.cpp Holes/Holes.h
        Lines/Lines.cpp Lines/Lines.h
        Edges/Edges.cpp Edges/Edges.h
        Isosurface/Isosurface.cpp Isosurface/Isosurface.h
        Cutters/Cutters.cpp Cutters/Cutters.h
        Splitter/Splitter.cpp Splitter/Splitter.h
        TestsAndExperiments/TestsAndExperiments.cpp TestsAndExperiments/TestsAndExperiments.h
        Distances/Distances.cpp Distances/Distances.h
        GeometricFigures/GeometricFigures.cpp GeometricFigures/GeometricFigures.h
        Rotation/Rotation.cpp Rotation/Rotation.h
        Objects_Geometry/PointsAndLines.cpp Objects_Geometry/PointsAndLines.h
)

target_link_libraries(VTK_Experiments PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(TARGETS VTK_Experiments MODULES ${VTK_LIBRARIES})