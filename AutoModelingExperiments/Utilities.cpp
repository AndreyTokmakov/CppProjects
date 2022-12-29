//============================================================================
// Name        : Utilities.cpp
// Created on  : 27.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <vtkDelaunay3D.h>
#include <vtkOutlineFilter.h>
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

#include "Utilities.h"

namespace Utilities {

    bool isLowerTooth([[maybe_unused]] uint_fast16_t id) {
        return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
    }

    bool isUpperTooth([[maybe_unused]] uint_fast16_t id) {
        return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
    }

    void WriteSTL(vtkAlgorithmOutput* data, std::string_view fileName) {
        vtkSmartPointer<vtkSTLWriter> writer { vtkSTLWriter::New() };
        writer->SetFileName(fileName.data());
        writer->SetInputConnection(data);
        writer->Write();
    }

    void WriteSTL(vtkSmartPointer<vtkPolyData> data, std::string_view fileName) {
        vtkSmartPointer<vtkSTLWriter> writer { vtkSTLWriter::New() };
        writer->SetFileName(fileName.data());
        writer->SetInputData(data);
        writer->Write();
    }

    vtkSmartPointer<vtkPolyData> readStl(std::string_view fileName) {
        vtkSmartPointer<vtkSTLReader> reader { vtkSTLReader::New() };
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> readObj(std::string_view fileName) {
        vtkSmartPointer<vtkOBJReader> reader { vtkOBJReader::New() };
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutput();
    }

    vtkAlgorithmOutput* readPolyData(std::string_view fileName)
    {
        const std::filesystem::path extension = std::filesystem::path(fileName).extension();

        if (extension == ".vtp") {
            vtkSmartPointer<vtkXMLPolyDataReader> reader { vtkXMLPolyDataReader::New() };
            reader->SetFileName(fileName.data());
            reader->Update();
            std::cout << "VTP\n";
            return reader->GetOutputPort();
        }

        vtkSmartPointer<vtkAbstractPolyDataReader> reader;
        if (extension == ".ply")
            reader = vtkPLYReader::New();
        else if (extension == ".obj")
            reader = vtkOBJReader::New();
        else if (extension == ".stl")
            reader = vtkSTLReader::New();
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutputPort();
    }

    vtkSmartPointer<vtkActor> getPolyDataActor(const vtkSmartPointer<vtkPolyData> polyData,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData(polyData);

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        return actor;
    }

    vtkSmartPointer<vtkActor> getPolyDataActor(const std::vector<vtkSmartPointer<vtkPolyData>>& data,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& mesh : data)
            dataAppender->AddInputData(mesh);
        dataAppender->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper{vtkPolyDataMapper::New()};
        mapper->SetInputData(dataAppender->GetOutput());

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        actor->GetProperty()->SetSpecular(0.3);
        actor->GetProperty()->SetSpecularPower(60.0);
        return actor;
    }

    vtkSmartPointer<vtkPolyData> pointsToPolyData(const std::vector<Types::Point<3, double>>& points)
    {
        vtkSmartPointer<vtkPoints> pts { vtkPoints::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> pointsPolydata { vtkPolyData::New() };
        pointsPolydata->SetPoints(pts);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter { vtkVertexGlyphFilter::New() };
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> polydata { vtkPolyData::New() };
        polydata->ShallowCopy(vertexFilter->GetOutput());
        return polydata;
    }

    vtkSmartPointer<vtkActor> getPointsActor(const std::vector<Types::Point<3, double>>& points,
                                             const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPoints> pts { vtkPoints::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> pointsPolydata { vtkPolyData::New() };
        pointsPolydata->SetPoints(pts);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter { vtkVertexGlyphFilter::New() };
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> polydata { vtkPolyData::New() };
        polydata->ShallowCopy(vertexFilter->GetOutput());

        vtkSmartPointer<vtkPolyDataMapper> mapperPoints { vtkPolyDataMapper::New() };
        mapperPoints->SetInputData(polydata);

        vtkSmartPointer<vtkActor> actorPoints { vtkActor::New() };
        actorPoints->SetMapper(mapperPoints);
        actorPoints->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoints->GetProperty()->SetPointSize(12);
        actorPoints->GetProperty()->RenderPointsAsSpheresOn();
        return actorPoints;
    }

    vtkSmartPointer<vtkActor> getLinesActor_FromPoints(const std::vector<Types::Point<3, double>>& points,
                                                       const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPoints> pts { vtkPoints::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> linesPolyData { vtkPolyData::New() };
        linesPolyData->SetPoints(pts);

        std::vector<vtkSmartPointer<vtkLine>> lines;
        for (size_t i = 0; i < points.size(); i += 2) {
            const auto line = lines.emplace_back (vtkNew<vtkLine>());
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
        }

        vtkSmartPointer<vtkCellArray> cellArray { vtkCellArray::New() };
        for (const auto& line : lines)
            cellArray->InsertNextCell(line);

        linesPolyData->SetLines(cellArray);

        const vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData(linesPolyData);

        vtkSmartPointer<vtkActor> actor{ vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetLineWidth(4);
        actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        return actor;
    }

    void DisplayActors(const std::vector<vtkSmartPointer<vtkActor>>& actors,
                       const vtkSmartPointer<vtkNamedColors>& colors) {
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());
        for (const auto& actor : actors)
            renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Window");

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    // TODO: Remove ??? Refactor
    void DisplayActorsEx(const std::vector<vtkSmartPointer<vtkProp3D>>& actors,
                         const vtkSmartPointer<vtkNamedColors>& colors) {
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());
        for (const auto& actor : actors)
            renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Window");

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(vtkAlgorithmOutput* data)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputConnection(data);

        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        actor->GetProperty()->SetSpecular(0.3);
        actor->GetProperty()->SetSpecularPower(60.0);

        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(800, 800);
        window->SetPosition(0, 50);
        window->AddRenderer(renderer);
        window->SetWindowName("ReadSTL"); // TODO: Rename

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);

        renderer->AddActor(actor);
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(vtkSmartPointer<vtkPolyData> data,
                   bool drawBoundingBox)
    {
        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1400, 900);
        window->SetPosition(200, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
            mapper->SetInputData(data);

            vtkSmartPointer<vtkActor> actor { vtkActor::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);
        }

        if (drawBoundingBox) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
            vtkSmartPointer<vtkOutlineFilter> outline { vtkOutlineFilter::New() };
            outline->SetInputData(data);
            mapper->SetInputConnection(outline->GetOutputPort());

            vtkSmartPointer<vtkActor> actor { vtkActor::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

            renderer->AddActor(actor);
        }

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);
        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(const std::vector<vtkSmartPointer<vtkPolyData>>& data,
                   bool drawBoundingBox)
    {
        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1000, 800);
        window->SetPosition(0, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        for (const auto& mesh : data) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
            mapper->SetInputData(mesh);

            vtkSmartPointer<vtkActor> actor { vtkActor::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);

            if (false == drawBoundingBox)
                continue;

            vtkSmartPointer<vtkOutlineFilter> outlineFilter { vtkOutlineFilter::New() };
            outlineFilter->SetInputData(mesh);

            vtkSmartPointer<vtkPolyDataMapper> outlineMapper { vtkPolyDataMapper::New() };
            outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());

            vtkSmartPointer<vtkActor> outlineActor { vtkActor::New() };
            outlineActor->SetMapper(outlineMapper);
            outlineActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            renderer->AddActor(outlineActor);
        }

        window->Render();
        vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor { vtkRenderWindowInteractor::New() };
        windowInteractor->SetRenderWindow(window);
        windowInteractor->Start();
    }

    void visualize(const std::vector<vtkAlgorithmOutput*>& data)
    {
        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1000, 800);
        window->SetPosition(0, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        for (const auto& mesh : data) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
            mapper->SetInputConnection(mesh);

            vtkSmartPointer<vtkActor> actor { vtkActor::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);
        }

        window->Render();
        vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor { vtkRenderWindowInteractor::New() };
        windowInteractor->SetRenderWindow(window);
        windowInteractor->Start();
    }

    void separate_connected_components(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter { vtkPolyDataConnectivityFilter::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();

        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();
        for (int i = 0; i < regionsCount; ++i) {
            vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter { vtkPolyDataConnectivityFilter::New() };
            singleRegionFilter->SetInputData(dataPtr);
            singleRegionFilter->SetExtractionModeToSpecifiedRegions();
            singleRegionFilter->AddSpecifiedRegion(i);
            singleRegionFilter->Update();

            visualize(singleRegionFilter->GetOutputPort());
        }
    }

    void separate_connected_components_ex(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter { vtkPolyDataConnectivityFilter::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();
        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();

        connectFilter->SetExtractionModeToSpecifiedRegions();
        connectFilter->Update();

        for (int i = 0; i < regionsCount; ++i) {
            connectFilter->InitializeSpecifiedRegionList();
            connectFilter->AddSpecifiedRegion(i);
            connectFilter->Update();

            visualize(connectFilter->GetOutputPort());
        }
    }

    void separate_connected_components_parallel_BAD(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        // TODO: We have a lot of noises when processing data in multithreaded manner
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter { vtkPolyDataConnectivityFilter::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();

        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();
        std::atomic<int> counter {0};
        auto task = [&]()  {
            while (regionsCount > counter) {
                const int regionId {counter++};
                vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter { vtkPolyDataConnectivityFilter::New() };
                singleRegionFilter->SetInputData(dataPtr);
                singleRegionFilter->SetExtractionModeToSpecifiedRegions();
                singleRegionFilter->AddSpecifiedRegion(regionId);
                singleRegionFilter->Update();
                //visualize(singleRegionFilter->GetOutputPort());

                vtkSmartPointer<vtkPolyData> pd_out { vtkPolyData::New() };
                pd_out->DeepCopy(singleRegionFilter->GetOutput());

                std::string fileName("/home/andtokm/Projects/data/out/tmp/tooth_" + std::to_string(regionId) + ".stl");

                vtkSmartPointer<vtkSTLWriter> stlWriter { vtkSTLWriter::New() };
                stlWriter->SetFileName(fileName.data());
                stlWriter->SetInputData(pd_out);
                stlWriter->Write();
                // Utilities::WriteSTL(singleRegionFilter->GetOutputPort(), destFile);
            }
        };

        std::vector<std::future<void>> workers;
        const unsigned int threadsCount = 6;//std::thread::hardware_concurrency();
        for (unsigned int i = 0; i < threadsCount; ++i) {
            workers.emplace_back(std::async(task));
        }
        std::for_each(workers.cbegin(), workers.cend(), [](const auto& s) { s.wait(); } );
        std::cout << "Done" << std::endl;
    }

    std::vector<vtkSmartPointer<vtkPolyData>> separate_connected_components_parallel_test(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter { vtkPolyDataConnectivityFilter::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();

        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();

        // TODO: Check performance at return from function
        std::vector<vtkSmartPointer<vtkPolyData>> teethsVector;
        teethsVector.reserve(regionsCount);
        std::atomic<int> counter {0};
        auto task = [&]()  {
            int regionId = counter.fetch_add(1, std::memory_order_release);
            while (regionsCount > regionId) {
                vtkSmartPointer<vtkPolyData> dataCopy { vtkPolyData::New() };
                dataCopy->DeepCopy(connectFilter->GetOutput());

                vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter { vtkPolyDataConnectivityFilter::New() };
                singleRegionFilter->SetInputData(dataCopy);
                singleRegionFilter->SetExtractionModeToSpecifiedRegions();
                singleRegionFilter->AddSpecifiedRegion(regionId);
                singleRegionFilter->Update();

                teethsVector.emplace_back(singleRegionFilter->GetOutput());
                regionId = counter.fetch_add(1, std::memory_order_release);
            }
        };

        std::vector<std::future<void>> workers;
        const unsigned int threadsCount = std::thread::hardware_concurrency();
        for (unsigned int i = 0; i < threadsCount / 2; ++i)
            workers.emplace_back(std::async(task));
        std::for_each(workers.cbegin(), workers.cend(), [](const auto& s) { s.wait(); } );
        // workers.clear();

        // TODO: Check performance at return from function
        //       move ?? or copy elision ???
        return teethsVector;

        /*
		int i = 0;
		for (const auto& data: teethsVector) {
			const std::string fileName("/home/andtokm/Projects/data/out/tmp/tooth_" + std::to_string(++i) + ".stl");
			Utilities::WriteSTL(data, fileName);
			// visualize(data);
		}
		std::cout << "Done" << std::endl;
        */
    }
}