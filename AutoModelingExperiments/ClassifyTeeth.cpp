//============================================================================
// Name        : classify.cpp
// Created on  : 27.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Classify src, research and experiments
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <vtkDelaunay3D.h>
#include <vtkAppendFilter.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>
#include <vtkConnectivityFilter.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
// #include <vtksys/SystemTools.hxx>
#include <vtkGenericCell.h>
#include <vtkBoxWidget.h>
#include <vtkColor.h>
#include <vtkCellIterator.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkMetaImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkWarpVector.h>
#include <vtkFillHolesFilter.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkLine.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSTLWriter.h>
#include <vtkTriangle.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWarpScalar.h>
#include <vtkProbeFilter.h>
#include <vtkXMLPolyDataWriter.h>


#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>
#include <filesystem>
#include <math.h>

#include <thread>
#include <future>

#include <vector>
#include <string>
#include <string_view>
#include <mutex>

#include "ClassifyTeeth.h"
#include "Utilities.h"

namespace ClassifyTeeth {

    void Test()
    {
        // constexpr std::string_view objFile{ R"(/home/andtokm/Projects/teeth_movement/example/data/P-497_-_upper_-_01_-_Model.stl)" };
        constexpr std::string_view lowerFilePath{
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_lower.stl)"};
        // constexpr std::string_view objFile{ R"("/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_upper.stl)" };

        // Utilities::visualize(Utilities::readPolyData(lowerFilePath.data()));
        // vtkSmartPointer<vtkPolyData> dataPtr = Utilities::readPolyData(lowerFilePath.data());

        vtkNew<vtkSTLReader> reader;
        reader->SetFileName(lowerFilePath.data());
        reader->Update();

        std::vector<vtkSmartPointer<vtkPolyData>> data = Utilities::separate_connected_components_parallel_test(
                reader->GetOutput());

        std::cout << data.size() << std::endl;
        for (auto &s: data) {
            Utilities::visualize(s);
        }
    }
}

void ClassifyTeeth::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    Test();
}