//============================================================================
// Name        : Utilities.cpp
// Created on  : 27.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "../VtkHeaders.h"

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


// TODO:
/*
Change:
        vtkSmartPointer<vtkObject> MyObject = vtkObject::New();
          --->
        vtkSmartPointer<vtkObject> MyObject = vtkSmartPointer<vtkObject>::New();
*/

namespace Utilities {

    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

    void WriteSTL(vtkAlgorithmOutput* data, std::string_view fileName) {
        vtkSmartPointer<vtkSTLWriter> writer { vtkSmartPointer<vtkSTLWriter>::New() };
        writer->SetFileName(fileName.data());
        writer->SetInputConnection(data);
        writer->Write();
    }

    void WriteSTL(vtkSmartPointer<vtkPolyData> data, std::string_view fileName) {
        vtkSmartPointer<vtkSTLWriter> writer { vtkSmartPointer<vtkSTLWriter>::New() };
        writer->SetFileName(fileName.data());
        writer->SetInputData(data);
        writer->Write();
    }

    vtkSmartPointer<vtkPolyData> readStl(std::string_view fileName) {
        vtkSmartPointer<vtkSTLReader> reader { vtkSmartPointer<vtkSTLReader>::New() };
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> readObj(std::string_view fileName) {
        vtkSmartPointer<vtkOBJReader> reader { vtkSmartPointer<vtkOBJReader>::New() };
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutput();
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData>& polyData,
                                                double x, double y, double z) noexcept {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->RotateX(x);
        transform->RotateY(y);
        transform->RotateZ(z);

        const vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter {
                vtkSmartPointer<vtkTransformPolyDataFilter>::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData>& polyData,
                                                const vtkSmartPointer<vtkMatrix4x4>& matrix) noexcept {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->SetMatrix(matrix);

        const vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter {
                vtkSmartPointer<vtkTransformPolyDataFilter>::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> moveTooth(const vtkSmartPointer<vtkPolyData>& polyData,
                                           double x, double y, double z) {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->Translate(x, y, z);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter
                { vtkSmartPointer<vtkTransformPolyDataFilter>::New()};
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> setPolyDataCenter(const vtkSmartPointer<vtkPolyData>& polyData,
                                                   double x, double y, double z) {
        Eigen::Vector3d center;
        polyData->GetCenter(center.data());
        return moveTooth(polyData, x - center[0], y - center[1], z - center[2]);
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> setPolyDataCenter(const vtkSmartPointer<vtkPolyData>& polyData,
                                                   const Eigen::Vector3d& pt) {
        Eigen::Vector3d center;
        polyData->GetCenter(center.data());
        return moveTooth(polyData, pt.x() - center.x(), pt.y() - center.y(), pt.z() - center.z());
    }

    vtkAlgorithmOutput* readPolyData(std::string_view fileName)
    {
        const std::filesystem::path extension = std::filesystem::path(fileName).extension();

        if (extension == ".vtp") {
            vtkSmartPointer<vtkXMLPolyDataReader> reader { vtkSmartPointer<vtkXMLPolyDataReader> ::New() };
            reader->SetFileName(fileName.data());
            reader->Update();
            std::cout << "VTP\n";
            return reader->GetOutputPort();
        }

        vtkSmartPointer<vtkAbstractPolyDataReader> reader;
        if (extension == ".ply")
            reader = vtkSmartPointer<vtkPLYReader>::New();
        else if (extension == ".obj")
            reader = vtkSmartPointer<vtkOBJReader>::New();
        else if (extension == ".stl")
            reader = vtkSmartPointer<vtkSTLReader>::New();
        reader->SetFileName(fileName.data());
        reader->Update();
        return reader->GetOutputPort();
    }

    vtkSmartPointer<vtkActor> getPolyDataActor(const vtkSmartPointer<vtkPolyData>& polyData)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper> ::New() };
        mapper->SetInputData(polyData);

        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapper);
        return actor;
    }

    vtkSmartPointer<vtkActor> getPolyDataActor(const std::vector<vtkSmartPointer<vtkPolyData>>& data)
    {
        vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkSmartPointer<vtkAppendPolyData>::New() };
        for (const auto& mesh : data)
            dataAppender->AddInputData(mesh);
        dataAppender->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        mapper->SetInputData(dataAppender->GetOutput());

        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        actor->GetProperty()->SetSpecular(0.3);
        actor->GetProperty()->SetSpecularPower(60.0);
        return actor;
    }

    vtkSmartPointer<vtkPolyData> pointsToPolyData(const std::vector<Eigen::Vector3d>& points)
    {
        vtkSmartPointer<vtkPoints> pts { vtkSmartPointer<vtkPoints>::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> pointsPolydata {  vtkSmartPointer<vtkPolyData>::New() };
        pointsPolydata->SetPoints(pts);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter { vtkSmartPointer<vtkVertexGlyphFilter>::New() };
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> polydata {  vtkSmartPointer<vtkPolyData>::New() };
        polydata->ShallowCopy(vertexFilter->GetOutput());
        return polydata;
    }

    vtkSmartPointer<vtkActor> getPointsActor(const std::vector<Eigen::Vector3d>& points,
                                             const std::array<double, 3>& color,
                                             const unsigned short size)
    {
        vtkSmartPointer<vtkPoints> pts { vtkSmartPointer<vtkPoints> ::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> pointsPolydata { vtkSmartPointer<vtkPolyData>::New() };
        pointsPolydata->SetPoints(pts);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter { vtkSmartPointer<vtkVertexGlyphFilter>::New() };
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> polydata { vtkSmartPointer<vtkPolyData>::New() };
        polydata->ShallowCopy(vertexFilter->GetOutput());

        vtkSmartPointer<vtkPolyDataMapper> mapperPoints { vtkSmartPointer<vtkPolyDataMapper>::New() };
        mapperPoints->SetInputData(polydata);

        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapperPoints);
        actor->GetProperty()->SetColor(color[0], color[1], color[2]);
        actor->GetProperty()->SetPointSize(size);
        actor->GetProperty()->RenderPointsAsSpheresOn();
        return actor;
    }

    vtkSmartPointer<vtkActor>
    getLinesActor_FromPoints(const std::vector<Eigen::Vector3d>& points)
    {
        vtkSmartPointer<vtkPoints> pts { vtkSmartPointer<vtkPoints>::New() };
        for (const auto& pt : points)
            pts->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> linesPolyData {  vtkSmartPointer<vtkPolyData>::New() };
        linesPolyData->SetPoints(pts);

        std::vector<vtkSmartPointer<vtkLine>> lines;
        for (size_t i = 0; i < points.size(); i += 2) {
            const auto line = lines.emplace_back (vtkNew<vtkLine>());
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
        }

        vtkSmartPointer<vtkCellArray> cellArray { vtkSmartPointer<vtkCellArray>::New() };
        for (const auto& line : lines)
            cellArray->InsertNextCell(line);

        linesPolyData->SetLines(cellArray);

        const vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        mapper->SetInputData(linesPolyData);

        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetLineWidth(4);
        actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        return actor;
    }

    vtkSmartPointer<vtkActor> getLineActor(const Eigen::Vector3d& pt1,
                                           const Eigen::Vector3d& pt2,
                                           const std::array<double, 3>& color)
    {
        vtkSmartPointer<vtkLineSource> lineSource { vtkSmartPointer<vtkLineSource>::New() };
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        lineSource->SetPoint1(pt1.data());
        lineSource->SetPoint2(pt2.data());
        lineSource->Update();
        mapper->SetInputData( lineSource->GetOutput() );
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(color[0], color[1], color[2]);
        actor->GetProperty()->SetLineWidth(3);
        return actor;
    }

    [[nodiscard("Make sure to handle return value")]]
    vtkSmartPointer<vtkActor> getOutlineActor(const vtkSmartPointer<vtkPolyData>& toothData)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        vtkSmartPointer<vtkOutlineFilter> outline { vtkSmartPointer<vtkOutlineFilter>::New() };
        outline->SetInputData(toothData);
        mapper->SetInputConnection(outline->GetOutputPort());

        vtkSmartPointer<vtkActor> outlineActor { vtkSmartPointer<vtkActor>::New() };
        outlineActor->SetMapper(mapper);
        outlineActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        return outlineActor;
    }

    void DisplayActors(const std::vector<vtkSmartPointer<vtkActor>>& actors,
                       const std::array<int, 2>& position,
                       const std::string_view windowName)
    {
        vtkSmartPointer<vtkRenderer> renderer { vtkSmartPointer<vtkRenderer>::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());
        for (const auto& actor : actors)
            renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(1500, 900);
        window->SetPosition(position[0], position[1]);
        window->AddRenderer(renderer);
        window->SetWindowName(windowName.data());

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor {
            vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    void DisplayActorsEx(const std::vector<vtkSmartPointer<vtkProp3D>>& actors,
                         const std::array<int, 2>& position,
                         const std::string_view windowName)
    {
        vtkSmartPointer<vtkRenderer> renderer { vtkSmartPointer<vtkRenderer>::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());
        for (const auto& actor : actors)
            renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(1500, 900);
        window->SetPosition(position[0], position[1]);
        window->AddRenderer(renderer);
        window->SetWindowName(windowName.data());

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor {
                vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(vtkAlgorithmOutput* data)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        mapper->SetInputConnection(data);

        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        actor->GetProperty()->SetSpecular(0.3);
        actor->GetProperty()->SetSpecularPower(60.0);

        vtkSmartPointer<vtkRenderer> renderer { vtkSmartPointer<vtkRenderer>::New() };
        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(800, 800);
        window->SetPosition(0, 50);
        window->AddRenderer(renderer);
        window->SetWindowName("ReadSTL"); // TODO: Rename

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor {
                vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        renderWindowInteractor->SetRenderWindow(window);

        renderer->AddActor(actor);
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(vtkSmartPointer<vtkPolyData> data,
                   bool drawBoundingBox)
    {
        vtkSmartPointer<vtkRenderer> renderer { vtkSmartPointer<vtkRenderer>::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(1000, 800);
        window->SetPosition(0, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
            mapper->SetInputData(data);

            vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);
        }

        if (drawBoundingBox) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
            vtkSmartPointer<vtkOutlineFilter> outline { vtkSmartPointer<vtkOutlineFilter>::New() };
            outline->SetInputData(data);
            mapper->SetInputConnection(outline->GetOutputPort());

            vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

            renderer->AddActor(actor);
        }

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor {
                vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        renderWindowInteractor->SetRenderWindow(window);
        window->Render();
        renderWindowInteractor->Start();
    }

    void visualize(const std::vector<vtkSmartPointer<vtkPolyData>>& data,
                   bool drawBoundingBox)
    {
        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(1000, 800);
        window->SetPosition(0, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        for (const auto& mesh : data) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
            mapper->SetInputData(mesh);

            vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);

            if (false == drawBoundingBox)
                continue;

            vtkSmartPointer<vtkOutlineFilter> outlineFilter { vtkSmartPointer<vtkOutlineFilter>::New() };
            outlineFilter->SetInputData(mesh);

            vtkSmartPointer<vtkPolyDataMapper> outlineMapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
            outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());

            vtkSmartPointer<vtkActor> outlineActor { vtkSmartPointer<vtkActor>::New() };
            outlineActor->SetMapper(outlineMapper);
            outlineActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            renderer->AddActor(outlineActor);
        }

        window->Render();
        vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor {
                vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        windowInteractor->SetRenderWindow(window);
        windowInteractor->Start();
    }

    void visualize(const std::vector<vtkAlgorithmOutput*>& data)
    {
        vtkSmartPointer<vtkRenderer> renderer { vtkSmartPointer<vtkRenderer>::New() };
        renderer->SetBackground(colors->GetColor3d("DarkGray").GetData());

        vtkSmartPointer<vtkRenderWindow> window { vtkSmartPointer<vtkRenderWindow>::New() };
        window->SetSize(1000, 800);
        window->SetPosition(0, 0);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK");

        for (const auto& mesh : data) {
            vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
            mapper->SetInputConnection(mesh);

            vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
            actor->SetMapper(mapper);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(60.0);

            renderer->AddActor(actor);
        }

        window->Render();
        vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor {
                vtkSmartPointer<vtkRenderWindowInteractor>::New() };
        windowInteractor->SetRenderWindow(window);
        windowInteractor->Start();
    }

    void separate_connected_components(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter {
                vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();

        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();
        for (int i = 0; i < regionsCount; ++i) {
            vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter {
                    vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
            singleRegionFilter->SetInputData(dataPtr);
            singleRegionFilter->SetExtractionModeToSpecifiedRegions();
            singleRegionFilter->AddSpecifiedRegion(i);
            singleRegionFilter->Update();

            visualize(singleRegionFilter->GetOutputPort());
        }
    }

    void separate_connected_components_ex(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter {
                vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
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
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter {
                vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
        connectFilter->SetInputData(dataPtr);
        connectFilter->SetExtractionModeToAllRegions();
        connectFilter->Update();

        const int regionsCount = connectFilter->GetNumberOfExtractedRegions();
        std::atomic<int> counter {0};
        auto task = [&]()  {
            while (regionsCount > counter) {
                const int regionId {counter++};
                vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter {
                        vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
                singleRegionFilter->SetInputData(dataPtr);
                singleRegionFilter->SetExtractionModeToSpecifiedRegions();
                singleRegionFilter->AddSpecifiedRegion(regionId);
                singleRegionFilter->Update();
                //visualize(singleRegionFilter->GetOutputPort());

                vtkSmartPointer<vtkPolyData> pd_out {  vtkSmartPointer<vtkPolyData>::New() };
                pd_out->DeepCopy(singleRegionFilter->GetOutput());

                std::string fileName("/home/andtokm/Projects/data/out/tmp/tooth_" + std::to_string(regionId) + ".stl");

                vtkSmartPointer<vtkSTLWriter> stlWriter { vtkSmartPointer<vtkSTLWriter>::New() };
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

    std::vector<vtkSmartPointer<vtkPolyData>>
    separate_connected_components_parallel_test(const vtkSmartPointer<vtkPolyData>& dataPtr)
    {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter {
            vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
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
                vtkSmartPointer<vtkPolyData> dataCopy {  vtkSmartPointer<vtkPolyData>::New() };
                dataCopy->DeepCopy(connectFilter->GetOutput());

                vtkSmartPointer<vtkPolyDataConnectivityFilter> singleRegionFilter {
                        vtkSmartPointer<vtkPolyDataConnectivityFilter>::New() };
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

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix3x3> cast4x4MatrixTo3x3(const vtkSmartPointer<vtkMatrix4x4>& matrix) {
        vtkSmartPointer<vtkMatrix3x3> matrix3x3 { vtkSmartPointer<vtkMatrix3x3>::New() };
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                matrix3x3->SetElement(row, col, matrix->GetElement(row, col));
            }
        }
        return matrix3x3;
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix4x4> cast3x3MatrixToVTK4x4(const Eigen::Matrix3d& matrix){
        vtkSmartPointer<vtkMatrix4x4> vtkMatrix { vtkSmartPointer<vtkMatrix4x4>::New() };
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                vtkMatrix->SetElement(col, row,  matrix.data()[row * 3 + col]);
        return vtkMatrix;
    }
}