//============================================================================
// Name        : Experiments.cpp
// Created on  : 21.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling experiments features
//============================================================================

#include <vtkSliderRepresentation.h>
#include <vtkClipPolyData.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkWidgetRepresentation.h>
#include <vtkCutter.h>
#include <vtkProperty2D.h>
#include <vtkCutter.h>
#include <vtkSliderWidget.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkFeatureEdges.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkRendererCollection.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkMergePoints.h>
#include <vtkMarchingCubes.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkExtractEdges.h>
#include <vtkTextActor3D.h>
#include <vtkLineSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkOBBTree.h>
#include <vtkPlaneSource.h>
#include <vtkCenterOfMass.h>
#include <vtkPlane.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkBoundingBox.h>
#include <vtkTransformPolyDataFilter.h>
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
#include <vtkOutlineFilter.h>
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

#include "Types.h"
#include "Utilities.h"
#include "Experiments.h"
#include "TreatmentPlan/TreatmentPlan.h"

#include <iostream>
#include <iomanip>
#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>


namespace Experiments {
    const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    void Show_STL() {
        constexpr std::string_view objFile{ R"(/home/andtokm/Projects/data/out/Tooths/tooth_11.stl)" };
        std::cout << objFile << std::endl;
        Utilities::visualize(Utilities::readPolyData(objFile.data()));
    }

    void SeparateComponentsTests() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };

        // [[maybe_unused]]
        // auto data = Utilities::readPolyData(objFile.data());

        vtkNew<vtkOBJReader> reader;
        reader->SetFileName(fileName.data());
        reader->Update();

        Utilities::separate_connected_components(reader->GetOutput());
    }

    void SeparateComponentsTests2() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };

        // [[maybe_unused]]
        // auto data = Utilities::readPolyData(objFile.data());

        vtkNew<vtkOBJReader> reader;
        reader->SetFileName(fileName.data());
        reader->Update();

        Utilities::separate_connected_components_ex(reader->GetOutput());
    }

    void SeparateComponentsTests_Parallel() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(fileName);

        // Utilities::separate_connected_components_parallel_BAD(reader->GetOutput());
        Utilities::separate_connected_components_parallel_test(polyData);
    }

    void SeparateComponentsTests_ShowTwoToothAsOnePolyData() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        std::vector<vtkSmartPointer<vtkPolyData>> pdVector =
                Utilities::separate_connected_components_parallel_test(Utilities::readObj(fileName));

        pdVector.resize(2);
        vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& mesh : pdVector)
            dataAppender->AddInputData(mesh);
        dataAppender->Update();

        Utilities::visualize(dataAppender->GetOutput());

        /*
        vtkSmartPointer<vtkSTLWriter> writer { vtkSTLWriter::New() };
        writer->SetFileName(R"(/home/andtokm/Projects/data/out/Tooths/test.stl)");
        writer->SetInputData(dataAppender->GetOutput());
        writer->Write();
        */
    }

    void SeparatedComponents_FindClosetPoints() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/data/out/Tooths/Two_Connected_Tooth_1.stl)"};
        const vtkSmartPointer<vtkPolyData> data = Utilities::readStl(fileName);
        std::vector<vtkSmartPointer<vtkPolyData>> pdVector =
                Utilities::separate_connected_components_parallel_test(data);

        for (const auto& pd: pdVector)
            Utilities::visualize(pd);
    }

    void ConnectivityAnalysis() {
        vtkSmartPointer<vtkSphereSource> sphereSource =vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(10);
        sphereSource->SetThetaResolution(10);
        sphereSource->SetPhiResolution(10);
        sphereSource->Update();

        vtkSmartPointer<vtkConeSource> coneSource =vtkSmartPointer<vtkConeSource>::New();
        coneSource->SetRadius(5);
        coneSource->SetHeight(10);
        coneSource->SetCenter(25, 0, 0);
        coneSource->Update();

        vtkSmartPointer<vtkAppendPolyData> appendFilter =vtkSmartPointer<vtkAppendPolyData>::New();
        appendFilter->AddInputData(sphereSource->GetOutput());
        appendFilter->AddInputData(coneSource->GetOutput());
        appendFilter->Update();

        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
        connectivityFilter->SetInputData(appendFilter->GetOutput());
        connectivityFilter->SetExtractionModeToCellSeededRegions();
        connectivityFilter->AddSeed(0);
        connectivityFilter->Update();


        ////////////////////////////////////////////////////////////////////////////
        vtkSmartPointer<vtkPolyDataMapper> originalMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        originalMapper->SetInputConnection(appendFilter->GetOutputPort());
        originalMapper->Update();
        vtkSmartPointer<vtkActor> originalActor =vtkSmartPointer<vtkActor>::New();
        originalActor->SetMapper(originalMapper);

        vtkSmartPointer<vtkPolyDataMapper> extractedMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        extractedMapper->SetInputConnection(connectivityFilter->GetOutputPort());
        extractedMapper->Update();
        vtkSmartPointer<vtkActor> extractedActor =vtkSmartPointer<vtkActor>::New();
        extractedActor->SetMapper(extractedMapper);
        /////////////////////
        double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
        double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

        vtkSmartPointer<vtkRenderer> leftRenderer =vtkSmartPointer<vtkRenderer>::New();
        leftRenderer->SetViewport(leftViewport);
        leftRenderer->AddActor(originalActor);
        leftRenderer->SetBackground(1, 0, 0);

        vtkSmartPointer<vtkRenderer> rightRenderer =vtkSmartPointer<vtkRenderer>::New();
        rightRenderer->SetViewport(rightViewport);
        rightRenderer->AddActor(extractedActor);
        rightRenderer->SetBackground(0, 0, 0);

        vtkSmartPointer<vtkRenderWindow> renderWindow =vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(leftRenderer);
        renderWindow->AddRenderer(rightRenderer);
        renderWindow->SetSize(640, 320);
        renderWindow->Render();
        renderWindow->SetWindowName("PolyDataConnectedCompExtract");

        leftRenderer->ResetCamera();
        rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

        vtkSmartPointer<vtkRenderWindowInteractor> interactor =vtkSmartPointer<vtkRenderWindowInteractor>::New();
        interactor->SetRenderWindow(renderWindow);
        interactor->Initialize();
        interactor->Start();
    }

    void Split_Mesh_STL()
    {
        constexpr std::string_view fileName {
            R"(/home/andtokm/Projects/teeth_movement/example/data/P-497_-_upper_-_01_-_Model.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(fileName);

        vtkNew<vtkPolyDataConnectivityFilter> connectFilter;
        connectFilter->SetInputData(polyData);
        connectFilter->SetExtractionModeToAllRegions();
        // connectFilter->SetExtractionModeToSpecifiedRegions();
        // connectFilter->ColorRegionsOn();
        connectFilter->Update();

        std::cout << connectFilter->GetNumberOfExtractedRegions() << std::endl;
    }

    void Split_Mesh_OBJ()
    {
        constexpr std::string_view fileName {
            R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(fileName);

        vtkNew<vtkPolyDataConnectivityFilter> connectFilter;
        connectFilter->SetInputData(polyData);
        connectFilter->SetExtractionModeToAllRegions();
        // connectFilter->SetExtractionModeToSpecifiedRegions();
        // connectFilter->ColorRegionsOn();
        connectFilter->Update();

        std::cout << connectFilter->GetNumberOfExtractedRegions() << std::endl;
    }

    // TODO: refactor
    Types::Point<3, double> toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Types::Point<3, double> pt {};
        pt[0] = atof(strPoints.substr(0, pos1).data());
        pt[1] = atof(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atof(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: refactor
    std::array<int, 3> toCell(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        std::array<int, 3> pt {};
        pt[0] = atoi(strPoints.substr(0, pos1).data());
        pt[1] = atoi(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atoi(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: Refactor
    // TODO: Move to Utilities ?????
    // Read .obj file prepared by preprocessing.py module
    std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>
    ReadTeethObj(std::string_view path) noexcept
    {
        std::ifstream input(path.data());
        std::vector<std::string> lines;
        while(std::getline(input, lines.emplace_back())) { /** Read all file lines. **/ }
        input.close();

        const size_t size {lines.size()};
        constexpr size_t APPROX_DATA_BLOCK_SIZE = 32 * 2;

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap;
        std::vector<Types::Point<3, double>> points;
        points.reserve(APPROX_DATA_BLOCK_SIZE);

        for (size_t i {0}; i < size && 'g' == lines[i++][0]; /* skip. */) {
            /** For each tooth: **/
            while (i < size && 'o' == lines[i][0]) {
                const unsigned short toothId = static_cast<unsigned short>(atoi(std::string_view(lines[i]).substr(3, lines[i].size() - 3).data()));
                dataMap.emplace(toothId, vtkPolyData::New());
                ++i;

                for (/* use outer 'i' */; i < size && 'v' == lines[i][0]; ++i) {
                    const std::string_view pointsStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    points.push_back(toPoint(pointsStr));
                }

                vtkSmartPointer<vtkPoints> vtkPoints = vtkPoints::New();
                for (const Types::Point<3, double>& pt: points)
                    vtkPoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                const int pointsCount { static_cast<int>(points.size())};
                vtkSmartPointer<vtkCellArray> vtkCells = vtkCellArray::New();
                for (/* use outer 'i' */; i < size && 'f' == lines[i][0]; ++i) {
                    const std::string_view cellStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    const auto& [x,y,z] = toCell(cellStr);
                    vtkCells->InsertNextCell({pointsCount + x, pointsCount+ y, pointsCount + z});
                    // vtkCells->InsertNextCell(3, toCellEx(cellStr, pointsCount));
                }

                dataMap[toothId]->SetPoints(vtkPoints);
                dataMap[toothId]->SetPolys(vtkCells);
                points.clear();
            }
        }
        return dataMap;
    }

    void Get_Number_Of_Extracted_Regions() {

        // Create 3 sphere
        vtkNew<vtkSphereSource> sphere1;
        sphere1->SetCenter(2, 0, 0);
        sphere1->Update();

        vtkNew<vtkSphereSource> sphere2;
        sphere2->SetCenter(5, 0, 0);
        sphere2->Update();

        vtkNew<vtkSphereSource> sphere3;
        sphere3->SetCenter(15, 0, 0);
        sphere3->Update();

        vtkNew<vtkDelaunay3D> delaunay1;
        delaunay1->SetInputConnection(sphere1->GetOutputPort());
        delaunay1->Update();

        vtkNew<vtkDelaunay3D> delaunay2;
        delaunay2->SetInputConnection(sphere2->GetOutputPort());
        delaunay2->Update();

        vtkNew<vtkDelaunay3D> delaunay3;
        delaunay3->SetInputConnection(sphere3->GetOutputPort());
        delaunay3->Update();


        // Add all spheres to filter
        vtkNew<vtkAppendFilter> appendFilter;
        appendFilter->AddInputConnection(delaunay1->GetOutputPort());
        appendFilter->AddInputConnection(delaunay2->GetOutputPort());
        appendFilter->AddInputConnection(delaunay3->GetOutputPort());
        appendFilter->Update();


        // Tring to extract all them
        vtkNew<vtkConnectivityFilter> connectivityFilter;
        connectivityFilter->SetInputConnection(appendFilter->GetOutputPort());
        connectivityFilter->SetExtractionModeToAllRegions();
        connectivityFilter->ColorRegionsOn();
        connectivityFilter->Update();

        std::cout << connectivityFilter->GetNumberOfExtractedRegions() << std::endl;
    }


    namespace MatrixUtils
    {
        constexpr float degToRad(float angle) {
            constexpr float M = M_PI / 180;
            return angle * M;
        };

        /*
        void PrintVtkMatrix(const vtkMatrix4x4* matrix) {
            std::cout << "-----------------------------------------------------------------------------------\n";
            for (size_t n = 0; n < 4; ++n) {
                for (size_t m = 0; m < 4; ++m) {
                    std::cout << std::setprecision(12) << matrix->GetElement(m, n) <<  "   ";
                }
                std::cout << std::endl;
            }
        }
        */

        vtkSmartPointer<vtkMatrix4x4> GetRotationMatrix(float angle) {
            vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

            const float radians = degToRad(angle);
            const float cosinus = std::cos(radians);
            const float sinus   = std::sin(radians);

            /*
            // Rotate X        Y  X
            matrix->SetElement(1, 1, cosinus;
            matrix->SetElement(1, 2, -sinus);

            matrix->SetElement(2, 1, sinus);
            matrix->SetElement(2, 2, cosinus);
            */

            /*
            // Rotate Y        Y  X
            matrix->SetElement(0, 0, cosinus);
            matrix->SetElement(0, 2, sinus);

            matrix->SetElement(2, 0, -sinus);
            matrix->SetElement(2, 2, cosinus);
            */

            // Rotate Z        Y  X
            matrix->SetElement(0, 0, cosinus);
            matrix->SetElement(0, 1, -sinus);

            matrix->SetElement(1, 0, sinus);
            matrix->SetElement(1, 1, cosinus);

            return matrix;
        }

        vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixX(float angle) {
            const float cosinus = std::cos(degToRad(angle));
            const float sinus   = std::sin(degToRad(angle));

            vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
            matrix->SetElement(1, 1, cosinus);
            matrix->SetElement(1, 2, -sinus);
            matrix->SetElement(2, 1, sinus);
            matrix->SetElement(2, 2, cosinus);
            return matrix;
        }

        vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixY(float angle) {
            const float cosinus = std::cos(degToRad(angle));
            const float sinus   = std::sin(degToRad(angle));

            vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
            matrix->SetElement(0, 0, cosinus);
            matrix->SetElement(0, 2, sinus);
            matrix->SetElement(2, 0, -sinus);
            matrix->SetElement(2, 2, cosinus);
            return matrix;
        }

        vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixZ(float angle) {
            const float cosinus = std::cos(degToRad(angle));
            const float sinus   = std::sin(degToRad(angle));

            vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
            matrix->SetElement(0, 0, cosinus);
            matrix->SetElement(0, 1, -sinus);
            matrix->SetElement(1, 0, sinus);
            matrix->SetElement(1, 1, cosinus);
            return matrix;
        }
    }

    void Transform_Single_Tooth() {
        // constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/tmp/tooth_6.stl)"};
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_lower.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        // constexpr float angle = -70.0f;

        vtkNew<vtkTransform> transform;
        transform->RotateX(0);
        transform->RotateY(90);
        transform->RotateZ(90);

        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        vtkSmartPointer<vtkMatrix4x4> matrix = transform->GetMatrix();
        // MatrixUtils::PrintVtkMatrix(matrix);
        std::cout << *matrix << std::endl;

        // auto matrix2 = MatrixUtils::GetRotationMatrix(angle);
        // MatrixUtils::PrintVtkMatrix(matrix2);

        Utilities::visualize(transformFilter->GetOutput());
    }

    void Transform_Single_Tooth_MatrixManual()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/tmp/tooth_6.stl)"};
        constexpr float angle = 30.0f;
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        auto matrix = MatrixUtils::GetRotationMatrixY(angle);
        // MatrixUtils::PrintVtkMatrix(matrix);
        std::cout << *matrix << std::endl;

        vtkNew<vtkTransform> transform;
        vtkNew<vtkTransformPolyDataFilter> transformFilter;

        transform->SetMatrix(matrix);
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);

        transformFilter->Update();
        // pd->ShallowCopy(transformFilter->GetOutput());

        // Utilities::visualize(polyData);
        Utilities::visualize(transformFilter->GetOutput());
    }

    void Transform_Single_Tooth_MatrixManual_OBJ_CrownsScan()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/cases/13758/models/5fd2_scan_crown.obj)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        vtkNew<vtkTransform> transform;
        vtkNew<vtkTransformPolyDataFilter> transformFilter;

        transform->RotateX(90);
        transform->RotateY(180);
        transform->RotateZ(0);

        [[maybe_unused]]
        vtkMatrix4x4* matrix = transform->GetMatrix();
        std::cout << *matrix << std::endl;

        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        Utilities::visualize(transformFilter->GetOutput());
    }

    void Transform_Single_Tooth_MatrixManual_OBJ_CrownsScan_ManualMatrix()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/cases/13758/models/5fd2_scan_crown.obj)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        const vtkSmartPointer<vtkMatrix4x4> mX = MatrixUtils::GetRotationMatrixX(90);
        const vtkSmartPointer<vtkMatrix4x4> mY = MatrixUtils::GetRotationMatrixY(180);
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Multiply4x4(mX.Get(), mY.Get(), matrix.Get());

        vtkNew<vtkTransform> transform;
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transform->SetMatrix(matrix);

        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        Utilities::visualize(transformFilter->GetOutput());
    }

    void Transform_Crowns_STL() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_lower.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkTransform> transform;
        vtkNew<vtkTransformPolyDataFilter> transformFilter;

        //transform->SetMatrix(matrix);
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);

        transform->RotateX(0);
        transform->RotateY(90);
        transform->RotateZ(90);

        [[maybe_unused]]
        const vtkMatrix4x4* matrix = transform->GetMatrix();
        // MatrixUtils::PrintVtkMatrix(matrix);
        // std::cout << *matrix << std::endl;

        transformFilter->Update();
        // pd->ShallowCopy(transformFilter->GetOutput());

        // Utilities::visualize(polyData);
        Utilities::visualize(transformFilter->GetOutput());
    }


    void vtkMatrix4x4_Tests() {
        auto mX = MatrixUtils::GetRotationMatrixX(30);
        std::cout << "Rotation matrix X: " << *mX << std::endl;

        auto mY = MatrixUtils::GetRotationMatrixY(30);
        std::cout<< "Rotation matrix Y: " << *mY << std::endl;

        auto mZ = MatrixUtils::GetRotationMatrixZ(30);
        std::cout << "Rotation matrix X: " <<  *mZ << std::endl;

        vtkSmartPointer<vtkMatrix4x4> M = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Multiply4x4(mY, mZ, M);

        std::cout << "Rotation matrix XY: " <<  *M << std::endl;
    }

}

namespace Experiments::OBBTreeTests
{
    class vtkSliderCallback : public vtkCommand {
    public:
        static vtkSliderCallback* New() {
            return new vtkSliderCallback;
        }

        vtkSliderCallback(): OBBTree(0), Level(0), PolyData(0), Renderer(0) {
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*)
        {
            vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
            this->Level = vtkMath::Round(
                    static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())->GetValue());
            this->OBBTree->GenerateRepresentation(this->Level, this->PolyData);
            this->Renderer->Render();
        }

        vtkOBBTree* OBBTree;
        int Level;
        vtkPolyData* PolyData;
        vtkRenderer* Renderer;
    };

    void VisualizeOBBTree() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_1.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkPolyDataMapper> pointsMapper;
        pointsMapper->SetInputData(polyData);
        pointsMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> pointsActor;
        pointsActor->SetMapper(pointsMapper);
        pointsActor->GetProperty()->SetInterpolationToFlat();
        pointsActor->GetProperty()->SetColor(colors->GetColor4d("Yellow").GetData());
        pointsActor->GetProperty()->SetOpacity(.3);

        int maxLevel = 5;
        // Create the tree
        vtkNew<vtkOBBTree> obbTree;
        obbTree->SetDataSet(polyData);
        obbTree->SetMaxLevel(maxLevel);
        obbTree->BuildLocator();

        double corner[3] = {0.0, 0.0, 0.0};
        double max[3] = {0.0, 0.0, 0.0};
        double mid[3] = {0.0, 0.0, 0.0};
        double min[3] = {0.0, 0.0, 0.0};
        double size[3] = {0.0, 0.0, 0.0};

        obbTree->ComputeOBB(polyData, corner, max, mid, min, size);

        std::cout << "Corner:\t" << corner[0] << ", " << corner[1] << ", "
                  << corner[2] << std::endl
                  << "Max:\t" << max[0] << ", " << max[1] << ", " << max[2]
                  << std::endl
                  << "Mid:\t" << mid[0] << ", " << mid[1] << ", " << mid[2]
                  << std::endl
                  << "Min:\t" << min[0] << ", " << min[1] << ", " << min[2]
                  << std::endl
                  << "Size:\t" << size[0] << ", " << size[1] << ", " << size[2]
                  << std::endl;

        // Initialize the representation
        vtkNew<vtkPolyData> polydata;
        obbTree->GenerateRepresentation(0, polydata);

        vtkNew<vtkPolyDataMapper> obbtreeMapper;
        obbtreeMapper->SetInputData(polydata);

        vtkNew<vtkActor> obbtreeActor;
        obbtreeActor->SetMapper(obbtreeMapper);
        obbtreeActor->GetProperty()->SetInterpolationToFlat();
        obbtreeActor->GetProperty()->SetOpacity(.5);
        obbtreeActor->GetProperty()->EdgeVisibilityOn();
        obbtreeActor->GetProperty()->SetColor(
                colors->GetColor4d("SpringGreen").GetData());

        // A renderer and render window
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);

        // An interactor
        vtkNew<vtkInteractorStyleTrackballCamera> style;
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->SetInteractorStyle(style);

        // Add the actors to the scene
        renderer->AddActor(pointsActor);
        renderer->AddActor(obbtreeActor);
        renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
        renderer->UseHiddenLineRemovalOn();

        // Render an image (lights and cameras are created automatically)
        renderWindow->SetWindowName("VisualizeOBBTree");
        renderWindow->SetSize(600, 600);
        renderWindow->Render();

        vtkNew<vtkSliderRepresentation2D> sliderRep;
        sliderRep->SetMinimumValue(0);
        sliderRep->SetMaximumValue(obbTree->GetLevel());
        sliderRep->SetValue(obbTree->GetLevel() / 2);
        sliderRep->SetTitleText("Level");
        sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRep->GetPoint1Coordinate()->SetValue(.2, .2);
        sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRep->GetPoint2Coordinate()->SetValue(.8, .2);
        sliderRep->SetSliderLength(0.075);
        sliderRep->SetSliderWidth(0.05);
        sliderRep->SetEndCapLength(0.05);
        sliderRep->GetTitleProperty()->SetColor(colors->GetColor3d("Beige").GetData());
        sliderRep->GetCapProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
        sliderRep->GetSliderProperty()->SetColor(colors->GetColor3d("LightBlue").GetData());
        sliderRep->GetSelectedProperty()->SetColor(colors->GetColor3d("Violet").GetData());

        vtkNew<vtkSliderWidget> sliderWidget;
        sliderWidget->SetInteractor(renderWindowInteractor);
        sliderWidget->SetRepresentation(sliderRep);
        sliderWidget->SetAnimationModeToAnimate();
        sliderWidget->EnabledOn();

        vtkNew<vtkSliderCallback> callback;
        callback->OBBTree = obbTree;
        callback->PolyData = polydata;
        callback->Renderer = renderer;
        callback->Execute(sliderWidget, 0, 0);

        sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

        renderWindowInteractor->Initialize();
        renderWindow->Render();

        renderWindowInteractor->Start();
    }
}

namespace Experiments::BoundsAndBorders
{
    using Point3d = Types::Point<3, double>;

    void Draw_BoundingBox()
    {
        // constexpr std::string_view fileName{ R"(/home/andtokm/Projects/data/cases/13758/models/5fd2_scan_crown.obj)" };
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_4.stl)" };

        const vtkSmartPointer<vtkPolyData> data = Utilities::readStl(filePath);
        Utilities::visualize(data, true);
    }

    void Draw_BoundingBox_UsingPoints() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        constexpr std::string_view singleTooth { R"(/home/andtokm/Projects/data/out/13181/Tooth_1.stl)" };

        const vtkSmartPointer<vtkPolyData> toothPolyData = Utilities::readStl(singleTooth);
        // const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);
        // const std::vector<vtkSmartPointer<vtkPolyData>> teeth = Utilities::separate_connected_components_parallel_test(polyData);

        // const vtkSmartPointer<vtkPolyData> data = teeth[1];
        const vtkSmartPointer<vtkPolyData> data = toothPolyData;

        vtkBoundingBox boundingBox;
        for( int i = 0; i < data->GetNumberOfPoints(); ++i ){
            boundingBox.AddPoint(data->GetPoint( i ) );
        }
        double bounds[6] = { 0 };
        boundingBox.GetBounds( bounds );

        std::vector<Types::Point<3, double>> pts;
        for( int i = 0; i < 2; ++i ) {
            for( int j = 2; j < 4; ++j ) {
                for( int k = 4; k < 6; ++k ) {
                    pts.push_back({bounds[i], bounds[j], bounds[k]});
                }
            }
        }

        vtkSmartPointer<vtkPolyData> boundsPolydata = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints> boundsPoints = vtkSmartPointer<vtkPoints>::New();
        for( int i = 0 ; i < 8; ++i )
            boundsPoints->InsertNextPoint( pts[i].data() );
        boundsPolydata->SetPoints( boundsPoints );

        vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
        vtkIdType cell[2] = { 0, 1 };
        cells->InsertNextCell( 2, cell );
        cell[0] = 0; cell[1] = 2;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 2;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 1;
        cells->InsertNextCell( 2, cell );
        cell[0] = 4; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 4; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 7; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 7; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 1; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 0; cell[1] = 4;
        cells->InsertNextCell( 2, cell );
        cell[0] = 2; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 7;
        cells->InsertNextCell( 2, cell );
        boundsPolydata->SetLines( cells );

        vtkSmartPointer<vtkPolyDataMapper> boundsMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        boundsMapper->SetInputData( boundsPolydata );
        vtkSmartPointer<vtkActor> boundsActor =vtkSmartPointer<vtkActor>::New();
        boundsActor->SetMapper( boundsMapper );
        boundsActor->GetProperty()->SetColor( 1, 0, 0 );

        // Setup the window
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> window;
        window->AddRenderer(renderer);
        window->SetSize(1200, 800);
        window->SetPosition(0, 50);
        window->SetWindowName("Outline");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(window);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(data);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        renderer->AddActor(actor);
        renderer->AddActor(boundsActor);
        renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData()); // Background color white

        // Render and interact
        window->Render();
        renderWindowInteractor->Start();
    }

    void Split_And_TryDrawBoundingBox() {
        // constexpr std::string_view filePath { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        // const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/cases/13758/automodeling/crowns/13758_lower.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        const std::vector<vtkSmartPointer<vtkPolyData>> teeth =
                Utilities::separate_connected_components_parallel_test(polyData);

        Utilities::visualize(teeth, true);
    }

    Point3d GetCentroid(vtkSmartPointer<vtkPolyData> polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputData(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Point3d pt;
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    Point3d GetCentroid(vtkAlgorithmOutput* polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputConnection(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Point3d pt;
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    void Cutter_Tests_OLD()
    {
        /*
        vtkNew<vtkCubeSource> cube;
        cube->SetXLength(10);
        cube->SetYLength(10);
        cube->SetZLength(10);

        const Point3d center = GetCentroid(cube->GetOutputPort());
        Point3d normal {1,0,0};


        vtkSmartPointer<vtkPlaneSource> planeSource { vtkPlaneSource::New() };
        planeSource->SetCenter( center.coordinates[0],center.coordinates[1],center.coordinates[2]);
        planeSource->SetNormal( normal.coordinates[0],normal.coordinates[1],normal.coordinates[2]);
        planeSource->Update();

        std::vector data {
            cube->GetOutputPort(),
            planeSource->GetOutputPort(),
        };

        Utilities::visualize(data);
        */



        vtkSmartPointer<vtkConeSource> cone { vtkConeSource::New() };
        //cone->SetDirection( 1, 1, 0 );
        cone->Update();

        double boundingBox[6] = { 0 };
        cone->GetOutput()->GetBounds( boundingBox );
        Point3d pt0({boundingBox[0], boundingBox[3], boundingBox[5]});
        Point3d pt1({boundingBox[0], boundingBox[3], boundingBox[4]});
        Point3d pt2({boundingBox[0], boundingBox[2], boundingBox[4]});
        Point3d centerPt = pt0 + pt2;

        // centerPt /= 2; // TODO: implement

        centerPt[0] /= 2;
        centerPt[1] /= 2;
        centerPt[2] /= 2;



        Point3d center, max, mid, min, size;
        vtkSmartPointer<vtkOBBTree> obbTree { vtkOBBTree::New() };
        obbTree->ComputeOBB(cone->GetOutput(),
                            center.data(),
                            max.data(),
                            mid.data(),
                            min.data(),
                            size.data());

        vtkMath::Normalize( center.data() );
        vtkMath::Normalize( max.data() );
        vtkMath::Normalize( mid.data() );
        vtkMath::Normalize( min.data() );

        cout << "center: " << center;
        cout << "max: " << max;
        cout << "mid: " << mid;
        cout << "min: " << min;





        vtkSmartPointer<vtkPlaneSource> planeSource { vtkPlaneSource::New() };
        planeSource->SetCenter( centerPt.data() );
        planeSource->SetNormal( 1, 0, 0 );
        planeSource->Update();


        vtkSmartPointer<vtkPolyDataMapper> planeMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData( planeSource->GetOutput() );
        planeMapper->Update();

        vtkSmartPointer<vtkActor> planeActor { vtkActor::New() };
        planeActor->SetMapper( planeMapper );
        planeActor->GetProperty()->SetColor( 0, 1, 0 );


        vtkNew<vtkPlane> plane;
        plane->SetOrigin( centerPt.data() );
        plane->SetNormal( 1, 0, 0 );

        vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
        cutter->SetCutFunction( plane );
        cutter->SetInputData( cone->GetOutput() );
        cutter->Update();

        cout << "Cutter GetNumberOfPoints: " << cutter->GetOutput()->GetNumberOfPoints() << endl;
        vtkSmartPointer<vtkPolyDataMapper> cutterMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        cutterMapper->SetInputData( cutter->GetOutput() );

        vtkSmartPointer<vtkActor> cutterActor = vtkSmartPointer<vtkActor>::New();
        cutterActor->SetMapper( cutterMapper );
        cutterActor->GetProperty()->SetColor( 1, 0, 0 );



        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData( cone->GetOutput() );

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper( mapper );

        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->AddActor(actor);
        ///renderer->AddActor(planeActor);
        renderer->AddActor(cutterActor);
        renderer->SetBackground( 0, 0, 0 );

        vtkSmartPointer<vtkRenderWindow> renderWindow { vtkRenderWindow::New() };
        renderWindow->AddRenderer( renderer );

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->GetInteractorStyle()->SetCurrentRenderer( renderer );
        renderWindowInteractor->SetRenderWindow( renderWindow );

        renderer->ResetCamera();
        renderWindow->Render();
        renderWindowInteractor->Start();
    }
}

namespace Experiments::CovnexHull {

    void SingleTooth_ConvexHull_Test() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const auto data = Utilities::readStl(filePath);

        vtkSmartPointer<vtkDelaunay3D> delaunay = vtkSmartPointer<vtkDelaunay3D>::New();
        delaunay->SetInputData( data );
        delaunay->Update();

        vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
        surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
        surfaceFilter->Update();

        Utilities::visualize(surfaceFilter->GetOutput());
        Utilities::visualize(data);
    }
}


namespace Experiments::Holes {

    void SingleTooth_FillHolesTest() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_15.stl)"};
        const auto data = Utilities::readStl(filePath);

        vtkNew<vtkFillHolesFilter> fillHoles;
        fillHoles->SetInputData(data);
        fillHoles->SetHoleSize(1000.0);
        fillHoles->Update();

        // Make the triangle winding order consistent
        /*
        vtkNew<vtkPolyDataNormals> normals;
        normals->SetInputConnection(fillHoles->GetOutputPort());
        normals->ConsistencyOn();
        normals->SplittingOff();
        normals->Update();
        normals->GetOutput()->GetPointData()->SetNormals(data->GetPointData()->GetNormals());
        */

        Utilities::visualize(fillHoles->GetOutput());
        Utilities::visualize(data);
    }
}

namespace Experiments::LinearInterpolation
{
    template<typename _Ty>
    std::vector<Types::Point<3, _Ty>> Interpolate(const std::vector<Types::Point<3, _Ty>>& points)
    {
        _Ty xSum {}, ySum {}, x2Sum{}, xySum{};
        for (const auto& pt: points) {
            xSum = xSum + pt[0];                // calculate sigma(xi)
            ySum = ySum + pt[2];                // calculate sigma(yi)
            x2Sum = x2Sum + std::pow(pt[0], 2); // calculate sigma(x^2i)
            xySum = xySum + pt[0] * pt[2];      // calculate sigma(xi*yi)
        }

        const size_t size { points.size() };
        const _Ty a = (size * xySum - xSum * ySum) /(size * x2Sum - xSum * xSum);  // calculate slope
        const _Ty b = (x2Sum * ySum - xSum * xySum)/(size * x2Sum - xSum * xSum);  // calculate intercept

        std::vector<Types::Point<3, _Ty>> pts(points);
        for (size_t i = 0; i < size; ++i)
            pts[i][2] = (a * points[i][0] + b);
        return  pts;
    }

    void Interpolate_Points() {
        std::vector<Types::Point<3,double>> points {
                {-15.3918, 3.8887,  13.8971 },
                {-15.3918, 3.8887,  13.8971 },
                {-15.3374, 4.08237, 14.1145 },
                {-15.1991, 4.325,   14.3688 },
                {-15.0578, 4.51604, 14.5046 },
                {-14.9991, 4.62078, 14.6887 },
                {-14.7168, 4.86583, 14.9945 },
                {-14.5061, 4.89272, 15.3488 },
                {-14.2221, 5.04323, 15.2444 },
                {-13.9874, 5.11911, 15.4124 },
                {-13.7869, 5.16497, 15.7041 },
                {-13.5104, 5.27468, 15.7543 },
                {-13.5273, 5.24448, 16.1772 },
                {-13.2595, 5.35238, 16.1171 },
                {-13.0147, 5.42938, 16.1466 },
                {-12.8194, 5.49648, 16.5808 },
                {-12.5179, 5.61649, 16.9036 },
                {-12.5179, 5.61649, 16.9036 },
                {-12.2713, 5.64042, 17.2161 },
                {-12.0587, 5.63157, 17.2951 },
                {-12.0587, 5.63157, 17.2951 },
                {-11.8347, 5.59475, 17.3427 },
                {-11.7607, 5.47401, 17.4489 },
                {-11.7372, 5.20569, 17.5896 },
                {-11.7372, 5.20569, 17.5896 },
                {-11.3333, 4.86595, 17.7547 },
                {-11.3333, 4.86595, 17.7547 },
                {-11.1578, 4.40782, 17.9409 },
                {-11.1578, 4.40782, 17.9409 },
                {-10.8993, 4.23208, 18.0502 },
                {-10.8432, 4.0622 , 18.11 },
                {-10.6483, 4.01287, 18.1455 },
                {-9.81791, 3.74825, 17.846 },
                {-9.70709, 3.59701, 17.7418 },
                {-9.63578, 3.21673, 17.6865 },
                {-9.66328, 2.97656, 17.7162 },
                {-9.49559, 2.84812, 17.4512 },
                {-9.49559, 2.84812, 17.4512 },
                {-9.5804,  2.63298, 17.5612 },
                {-9.45364, 2.39051, 17.3248 }
        };

        std::for_each(points.begin(), points.end(), [](auto& pt) {pt[1] = 0;});
        const auto& pts = Interpolate(points);
        vtkSmartPointer<vtkActor> actorPoints = Utilities::getPointsActor(pts, colors);
        actorPoints->GetProperty()->SetPointSize(6);

        Utilities::DisplayActors({actorPoints}, colors);
    }
};

namespace Experiments::DrawText {

    vtkSmartPointer<vtkActor> getLineActor(const Types::Line<3>& line,
                                           const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkLineSource> lineSource {vtkLineSource::New()};
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        lineSource->SetPoint1(line.getFirstPoint().data());
        lineSource->SetPoint2(line.getSecondPoint().data()) ;
        lineSource->Update();
        mapper->SetInputData( lineSource->GetOutput() );
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor( 0, 1, 0 );
        actor->GetProperty()->SetLineWidth(4);
        return actor;
    }

    void ShowText() {
        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkSphereSource> sphereSource { vtkSphereSource::New() };
        sphereSource->SetCenter(0.0, 0.0, 0.0);
        sphereSource->SetRadius(5.0);
        sphereSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper{ vtkPolyDataMapper::New() };
        mapper->SetInputData(sphereSource->GetOutput());

        vtkSmartPointer<vtkActor> actor{ vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        vtkNew<vtkTextActor> textActor;
        textActor->SetInput("50");
        textActor->SetPosition(50, 50);
        textActor->GetTextProperty()->SetFontSize(24);
        textActor->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        vtkNew<vtkTextActor> textActor2;
        textActor2->SetInput("75");
        textActor2->SetPosition(120, 50);
        textActor2->GetTextProperty()->SetFontSize(24);
        textActor2->GetTextProperty()->SetColor(colors->GetColor3d("Green").GetData());


        vtkSmartPointer<vtkRenderer> renderer{ vtkRenderer::New() };
        // renderer->AddActor(actor);
        renderer->AddActor2D(textActor);
        renderer->AddActor2D(textActor2);
        renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

        // Create a render window
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
        renderWindow->SetWindowName("DrawText");

        // Create an interactor
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Render and interact
        renderWindow->Render();
        renderWindowInteractor->Start();
    }

    void ShowText2() {
        vtkNew<vtkVectorText> textSource;
        textSource->SetText("Hello");

        // Create a mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(textSource->GetOutputPort());

        // Create a subclass of vtkActor: a vtkFollower that remains facing the camera
        vtkNew<vtkFollower> follower;
        follower->SetMapper(mapper);
        follower->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        // Create a transparent cube that does not move around to face the camera
        vtkNew<vtkCubeSource> cubeSource;
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(colors->GetColor3d("MidnightBlue").GetData());
        cubeActor->GetProperty()->SetOpacity(0.3);

        // Create a renderer, render window, and interactor
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
        renderWindow->SetWindowName("Follower");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        dynamic_cast<vtkInteractorStyle*>(renderWindowInteractor->GetInteractorStyle())->AutoAdjustCameraClippingRangeOn();
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Add the actor to the scene
        renderer->AddActor(follower);
        renderer->AddActor(cubeActor);
        renderer->SetBackground(colors->GetColor3d("LightSlateGray").GetData());
        follower->SetCamera(renderer->GetActiveCamera());

        // Render and interact
        renderWindow->Render();
        renderer->ResetCamera();
        renderWindow->Render();
        renderWindowInteractor->Start();
    }

    void DrawLine_And_ShowText() {
        constexpr size_t fontSize {18};
        Types::Point<3, double> pt1 {0, 10, 0}, pt2 {300, 10, 0};
        const int lineLengthActual = static_cast<int>(pt1.distanceTo(pt2));
        const auto midPoint = Types::Point<3, double>::midPoint(pt1, pt2);
        std::string lineText {std::to_string(lineLengthActual) + " / 1234"};



        const vtkSmartPointer<vtkActor> lineActor = getLineActor({pt1, pt2}, colors);
        const vtkSmartPointer<vtkTextActor3D> textActor3D { vtkTextActor3D::New() };
        textActor3D->SetPosition(midPoint[0] - lineText.size() * fontSize / 4, midPoint[1] + 3, midPoint[2]);
        textActor3D->SetInput(lineText.c_str());
        textActor3D->GetTextProperty()->SetFontSize(fontSize);
        textActor3D->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        Utilities::DisplayActorsEx({lineActor, textActor3D}, colors);
    }

    void DrawLineWithTextSizes() {
        constexpr size_t fontSize {16};
        constexpr size_t expectedSize {123};                       /** INPUT VALUE **/
        Types::Point<3, double> pt1 {0, 10, 0}, pt2 {300, 10, 0};  /** INPUT VALUE **/

        const vtkSmartPointer<vtkActor> lineActor = getLineActor({pt1, pt2}, colors);
        lineActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

        const int lineLengthActual = static_cast<int>(pt1.distanceTo(pt2));
        std::string lineText {std::to_string(lineLengthActual)};
        auto textPosPt = Types::Point<3, double>::midPoint(pt1, pt2);
        textPosPt[0] -= lineText.size() * fontSize / 4,
        textPosPt[1] += 3;

        const vtkSmartPointer<vtkTextActor3D> textActor1 { vtkTextActor3D::New() };
        textActor1->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor1->SetInput(lineText.c_str());
        textActor1->GetTextProperty()->SetFontSize(fontSize);
        textActor1->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        textPosPt[0] += lineText.size()  * fontSize / 2 + 2;

        const vtkSmartPointer<vtkTextActor3D> textActor2 { vtkTextActor3D::New() };
        textActor2->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor2->SetInput("/");
        textActor2->GetTextProperty()->SetFontSize(fontSize);
        textActor2->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        textPosPt[0] += 5;

        const vtkSmartPointer<vtkTextActor3D> textActor3 { vtkTextActor3D::New() };
        textActor3->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor3->SetInput(std::to_string(expectedSize).c_str());
        textActor3->GetTextProperty()->SetFontSize(fontSize);
        textActor3->GetTextProperty()->SetColor(colors->GetColor3d("Green").GetData());

        Utilities::DisplayActorsEx({lineActor, textActor1, textActor2, textActor3}, colors);
    }
}

namespace Experiments::MoveTeeth {

    vtkSmartPointer<vtkPolyData> transformData(const vtkSmartPointer<vtkPolyData> polyData) {
        const vtkSmartPointer<vtkMatrix4x4> mX = MatrixUtils::GetRotationMatrixX(90);
        const vtkSmartPointer<vtkMatrix4x4> mY = MatrixUtils::GetRotationMatrixY(180);
        const vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Multiply4x4(mX.Get(), mY.Get(), matrix.Get());

        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->SetMatrix(matrix);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> moveTooth(const vtkSmartPointer<vtkPolyData> polyData)
    {
        constexpr double a { -0.03065775028732241 };
        constexpr double b { -0.001127238341818489 };
        constexpr double c { 22.309923007969807 };
        auto getZ = [&] (double x) {
            return  a * x * x + b * x + c;
        };

        Types::Point<3, double> center;
        polyData->GetCenter(center.data());
        const double moveOnZ = getZ(center[0]) - center[2];
        std::cout << moveOnZ << std::endl;
        std::cout << center << std::endl;


        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->Translate(0, 0, moveOnZ);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    void OriginPoints_TransformPoints()
    {
        const TreatmentPlan::Plan& plan =
                TreatmentPlan::Parser::Parse(R"(/home/andtokm/Projects/data/cases_ml/10066/Plan.json)");

        std::vector<Types::Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        const vtkSmartPointer<vtkPolyData> pointsData = Utilities::pointsToPolyData(points);
        const vtkSmartPointer<vtkPolyData> pointsDataTransformed = transformData(pointsData);
        const vtkSmartPointer<vtkPolyData> lowerTeethData =
                Utilities::readStl("/home/andtokm/Projects/data/cases_ml/10066/after/crowns/10066_lower.stl");
        const vtkSmartPointer<vtkPolyData> upperTeethData =
                Utilities::readStl("/home/andtokm/Projects/data/cases_ml/10066/after/crowns/10066_upper.stl");

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPolyDataActor(pointsDataTransformed, colors);
        vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(lowerTeethData, colors);
        vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(upperTeethData, colors);

        actorPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeethLower, actorTeethUpper}, colors);
    }



    void TryMoveTooth([[maybe_unused]] const std::vector<std::string_view>& params) {
        std::string folderName {R"(/home/andtokm/Projects/data/cases_ml/)"};
        folderName.append(params.front());

        std::string fileName {folderName};
        fileName.append(R"(/before/crowns/)").append(params.front()).append("_upper.stl");

        std::string destFileName {folderName};
        destFileName.append(R"(/)").append(params.front()).append("_upper_test.stl");

        const vtkSmartPointer<vtkPolyData> teethPolyData = Utilities::readStl(fileName);

        std::vector<Types::Point<3, double>> points;
        teethPolyData->GetCenter(points.emplace_back().data());
        const vtkSmartPointer<vtkPolyData> pointsData = Utilities::pointsToPolyData(points);

        std::vector<vtkSmartPointer<vtkPolyData>> teethData =
                Utilities::separate_connected_components_parallel_test(teethPolyData);

        Utilities::visualize(teethData);

        // Sort by distance on X from the center
        std::sort(teethData.begin(), teethData.end(), [&] (const auto& pd1, const auto& pd2) {
            const auto center1 = pd1->GetCenter();
            const auto center2 = pd2->GetCenter();
            const auto dataCenter = points.front();
            return std::abs(center1[0] - dataCenter[0]) < std::abs(center2[0] - dataCenter[0]);
        });

        std::vector<vtkSmartPointer<vtkPolyData>> tmp (teethData.begin(), teethData.begin() + 6);
        Utilities::visualize(tmp);


        for (int i = 0; i < 6; ++i)
            teethData[i] = moveTooth(teethData[i]);

        vtkSmartPointer<vtkActor> teethActor = Utilities::getPolyDataActor(teethData, colors);
        Utilities::DisplayActors({teethActor}, colors);

        vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& mesh : teethData)
            dataAppender->AddInputData(mesh);
        dataAppender->Update();

        vtkSmartPointer<vtkSTLWriter> writer { vtkSTLWriter::New() };
        writer->SetFileName(destFileName.c_str());
        writer->SetInputData(dataAppender->GetOutput());
        writer->Write();
    }
}

namespace Experiments::Lines {
    vtkSmartPointer<vtkActor> getLineActor(const Types::Line<3>& line,
                                           const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkLineSource> lineSource {vtkLineSource::New()};
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        lineSource->SetPoint1(line.getFirstPoint().data());
        lineSource->SetPoint2(line.getSecondPoint().data()) ;
        lineSource->Update();
        mapper->SetInputData( lineSource->GetOutput() );
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor( 0, 1, 0 );
        actor->GetProperty()->SetLineWidth(3);
        return actor;
    }

    void DrawLine()
    {
        Types::Point<3, double> pt1 {0, 0, 0};
        Types::Point<3, double> pt2 {10, 10, 10};

        const vtkSmartPointer<vtkActor> lineActor = getLineActor({pt1, pt2}, colors);

        Utilities::DisplayActors({ lineActor }, colors);
    }
}

namespace Experiments::Edges {

    void ExtractEdges() {
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->Update();

        vtkNew<vtkExtractEdges> extractEdges;
        extractEdges->SetInputConnection(sphereSource->GetOutputPort());
        extractEdges->Update();

        // Traverse all of the edges
        // vtkCellArray* lines = extractEdges->GetOutput()->GetLines();
        for (vtkIdType i = 0; i < extractEdges->GetOutput()->GetNumberOfCells(); i++) {
            // extractEdges->GetOutput()->GetCell(i)->GetClassName() << std::endl;
            auto line = dynamic_cast<vtkLine*>(extractEdges->GetOutput()->GetCell(i));
        }

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(extractEdges->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        Utilities::DisplayActors({ actor }, colors);
    }

    void ExtractEdges_Tooth() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkExtractEdges> extractEdges;
        extractEdges->SetInputData(polyData);
        // extractEdges->Nu
        extractEdges->Update();

        // Traverse all of the edges
        // vtkCellArray* lines = extractEdges->GetOutput()->GetLines();
        for (vtkIdType i = 0; i < extractEdges->GetOutput()->GetNumberOfCells(); i++) {
            // extractEdges->GetOutput()->GetCell(i)->GetClassName() << std::endl;
            auto line = dynamic_cast<vtkLine*>(extractEdges->GetOutput()->GetCell(i));
        }

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(extractEdges->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        Utilities::DisplayActors({ actor }, colors);
    }

    void ExtractFeatureEdges_Tooth() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(polyData);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOn();
        featureEdges->ManifoldEdgesOff();
        featureEdges->NonManifoldEdgesOn();
        featureEdges->Update();

        const vtkSmartPointer<vtkActor> actorTooth = Utilities::getPolyDataActor(polyData, colors);
        const vtkSmartPointer<vtkActor> actor =
                Utilities::getPolyDataActor(featureEdges->GetOutput(), colors);
        actor->GetProperty()->SetLineWidth(5);

        Utilities::DisplayActors({actor, actorTooth}, colors);
    }
}

namespace Experiments::Isosurface {

    void Test() {
        vtkNew<vtkRenderer> ren1;
        vtkNew<vtkRenderWindow> renWin;
        renWin->AddRenderer(ren1);

        vtkNew<vtkRenderWindowInteractor> iren;
        iren->SetRenderWindow(renWin);

        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkMergePoints> locator;
        locator->SetDivisions(64, 64, 92);
        locator->SetNumberOfPointsPerBucket(2);
        locator->AutomaticOff();

#ifdef USE_FLYING_EDGES
        vtkNew<vtkFlyingEdges3D> iso;
#else
        vtkNew<vtkMarchingCubes> iso;
#endif
        iso->SetInputData(polyData);
        iso->ComputeGradientsOn();
        iso->ComputeScalarsOff();
        iso->SetValue(0, 1150);
#ifndef USE_FLYING_EDGES
        iso->SetLocator(locator);
#endif

        vtkNew<vtkPolyDataMapper> isoMapper;
        isoMapper->SetInputConnection(iso->GetOutputPort());
        isoMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> isoActor;
        isoActor->SetMapper(isoMapper);
        isoActor->GetProperty()->SetColor(colors->GetColor3d("Ivory").GetData());

        vtkNew<vtkOutlineFilter> outline;
        outline->SetInputData(polyData);

        vtkNew<vtkPolyDataMapper> outlineMapper;
        outlineMapper->SetInputConnection(outline->GetOutputPort());

        vtkNew<vtkActor> outlineActor;
        outlineActor->SetMapper(outlineMapper);

        // Add the actors to the renderer, set the background and size
        //
        ren1->AddActor(outlineActor);
        ren1->AddActor(isoActor);
        ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
        ren1->GetActiveCamera()->SetFocalPoint(0, 0, 0);
        ren1->GetActiveCamera()->SetPosition(0, -1, 0);
        ren1->GetActiveCamera()->SetViewUp(0, 0, -1);
        ren1->ResetCamera();
        ren1->GetActiveCamera()->Dolly(1.5);
        ren1->ResetCameraClippingRange();

        renWin->SetSize(640, 480);
        renWin->SetWindowName("HeadBone");

        renWin->Render();
        iren->Start();
    }
}

namespace Experiments::UserInteraction
{

    class MouseInteractorStyle6 : public vtkInteractorStyleTrackballActor
    {
    public:
        static MouseInteractorStyle6* New();
    vtkTypeMacro(MouseInteractorStyle6, vtkInteractorStyleTrackballActor);

        virtual void OnLeftButtonDown() override
        {
            std::cout << "Pressed left mouse button." << std::endl;
            // Forward events
            vtkInteractorStyleTrackballActor::OnLeftButtonDown();
        }

        virtual void OnMiddleButtonUp() override
        {
            // std::cout << "Pressed middle mouse button." << std::endl;

            int x = this->Interactor->GetEventPosition()[0];
            int y = this->Interactor->GetEventPosition()[1];
            this->FindPokedRenderer(x, y);
            this->FindPickedActor(x, y);

            if (this->CurrentRenderer == NULL || this->InteractionProp == NULL) {
                std::cout << "Nothing selected." << std::endl;
                return;
            }

            vtkNew<vtkPropCollection> actors;

            this->InteractionProp->GetActors(actors);
            actors->InitTraversal();
            vtkActor* actor = dynamic_cast<vtkActor*>(actors->GetNextProp());
            vtkPolyData* polydata =dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInputAsDataSet());

            vtkNew<vtkTransform> transform;
            transform->SetMatrix(actor->GetMatrix());

            vtkNew<vtkTransformPolyDataFilter> transformPolyData;
            transformPolyData->SetInputData(polydata);
            transformPolyData->SetTransform(transform);
            transformPolyData->Update();

            vtkNew<vtkSelectEnclosedPoints> selectEnclosedPoints;
            selectEnclosedPoints->SetInputConnection(transformPolyData->GetOutputPort());
            selectEnclosedPoints->SetSurfaceData(this->Sphere);
            selectEnclosedPoints->Update();

            vtkDataArray* insideArray = dynamic_cast<vtkDataArray*>(
                    selectEnclosedPoints->GetOutput()->GetPointData()->GetArray("SelectedPoints"));

            bool inside = false;
            for (vtkIdType i = 0; i < insideArray->GetNumberOfTuples(); i++) {
                if (insideArray->GetComponent(i, 0) == 1) {
                    inside = true;
                    break;
                }
            }

            if (inside)
            {
                std::cout << "A point of the cube is inside the sphere!" << std::endl;
                // Reset the cube to its original position
                // this->CubeActor->GetMatrix()->Identity();
                // this->CubeActor->SetOrigin(0,0,0);
                this->CubeActor->SetPosition(0, 0, 0);
                this->CubeActor->SetOrientation(0, 0, 0);

                this->Interactor->GetRenderWindow()
                        ->GetRenderers()
                        ->GetFirstRenderer()
                        ->Render();
                this->Interactor->GetRenderWindow()->Render();
            }

            // Release interaction
            this->StopState();
        }

        virtual void OnRightButtonDown() override
        {
            std::cout << "Pressed right mouse button." << std::endl;
            // Forward events
            vtkInteractorStyleTrackballActor::OnRightButtonDown();
        }

        vtkPolyData* Sphere = nullptr;
        vtkActor* CubeActor = nullptr;
    };

    vtkStandardNewMacro(MouseInteractorStyle6);


    void InteractionTest() {
        vtkNew<vtkNamedColors> color;

        // Sphere
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetRadius(2);
        sphereSource->Update();

        vtkNew<vtkPolyDataMapper> sphereMapper;
        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

        vtkNew<vtkActor> sphereActor;
        sphereActor->SetMapper(sphereMapper);
        sphereActor->GetProperty()->SetColor(color->GetColor3d("Tomato").GetData());

        // Cube
        vtkNew<vtkCubeSource> cubeSource;
        cubeSource->SetCenter(5.0, 0.0, 0.0);
        cubeSource->Update();

        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(color->GetColor3d("DodgerBlue").GetData());

        // Visualize
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
        renderWindow->SetWindowName("Game");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(renderWindow);

        renderer->AddActor(sphereActor);
        renderer->AddActor(cubeActor);
        renderer->SetBackground(color->GetColor3d("Burlywood").GetData());

        renderWindow->Render();

        vtkNew<MouseInteractorStyle6> style;
        style->Sphere = sphereSource->GetOutput();
        style->CubeActor = cubeActor;

        renderWindowInteractor->SetInteractorStyle(style);

        renderWindowInteractor->Start();
    }
}

namespace Experiments::Cutters {

    Types::Point<3, double>  GetCentroid(vtkSmartPointer<vtkPolyData> polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputData(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Types::Point<3, double>  pt;
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    void SimpleTest1()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData, colors);

        Types::Point<3, double> centroid {};
        polyData->GetCenter(centroid.data());

        Types::Point<3, double> normals {0.0, 1.0, 0.0};
        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Types::Point<3, double> minBound {bounds[0], bounds[2], bounds[4]};
        Types::Point<3, double> maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = centroid[1] - minBound[1];
        const double maxDistZ = maxBound[1] - centroid[1];

        vtkNew<vtkPlane> plane;
        plane->SetOrigin(centroid.data());
        plane->SetNormal(normals.data());

        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);
        cutter->GenerateValues(100, -minDistZ, maxDistZ);
        cutter->Update();

        std::cout << "GetNumberOfContours = " << cutter->GetNumberOfContours() << std::endl;
        Utilities::visualize(cutter->GetOutput());
    }


    void CutToothWithPlane()
    {
        constexpr size_t numContours {50};
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData, colors);

        Types::Point<3, double> centroid {};
        polyData->GetCenter(centroid.data());

        // auto pointActor = Utilities::getPointsActor({centroid}, colors);
        // Utilities::DisplayActors({pointActor, toothActor}, colors);

        Types::Point<3, double> normals {0.0, 1.0, 0.0};
        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Types::Point<3, double> minBound {bounds[0], bounds[2], bounds[4]};
        Types::Point<3, double> maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = centroid[1] - minBound[1];
        const double maxDistZ = maxBound[1] - centroid[1];

        vtkNew<vtkPlane> plane;
        plane->SetOrigin(centroid.data());
        plane->SetNormal(normals.data());

        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);
        cutter->GenerateValues(numContours, /* -minDistZ */ maxDistZ/ 2, maxDistZ);
        cutter->Update();

        vtkNew<vtkStripper> contourStripper;
        contourStripper->SetInputConnection(cutter->GetOutputPort());
        contourStripper->JoinContiguousSegmentsOn();
        // contourStripper->JoinContiguousSegmentsOff();
        contourStripper->Update();


        vtkNew<vtkPolyDataMapper> cutterMapper;
        cutterMapper->SetInputConnection(contourStripper->GetOutputPort());
        cutterMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> planeActor;
        planeActor->GetProperty()->SetColor(colors->GetColor3d("Deep_pink").GetData());
        planeActor->GetProperty()->SetLineWidth(2);
        planeActor->SetMapper(cutterMapper);

        // Utilities::visualize(contourStripper->GetOutput());
        Utilities::DisplayActors({planeActor}, colors);
    }

    void Cut_Tooth_Vertical() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_1.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        // Create a plane to cut
        vtkNew<vtkPlane> plane;
        plane->SetOrigin(polyData->GetCenter());

        // TODO: Angle of the plane
        plane->SetNormal(0, 1, 0.1);

        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Types::Point<3, double> center {};
        polyData->GetCenter(center.data());

        Types::Point<3, double> minBound {bounds[0], bounds[2], bounds[4]};
        Types::Point<3, double> maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = center[1] - minBound[1];
        const double maxDistZ = maxBound[1] - center[1];
        const double beginDistZ = maxDistZ / 2; // Middle between center and upper point

        std::cout << "minBound: " << minBound << "\n"
                  << "maxBound: " << maxBound << "\n"
                  << "center: " << center << std::endl;
        std::cout << "distanceMin = " << minDistZ << "\n"
                  << "distanceMax = " << maxDistZ << std::endl;

        // Create cutter
        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);

        cutter->GenerateValues(30, beginDistZ - 2, maxDistZ + 2);
        vtkNew<vtkPolyDataMapper> cutterMapper;
        cutterMapper->SetInputConnection(cutter->GetOutputPort());
        cutterMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> planeActor;
        planeActor->GetProperty()->SetColor(colors->GetColor3d("Deep_pink").GetData());
        planeActor->GetProperty()->SetLineWidth(5);
        planeActor->SetMapper(cutterMapper);

        const vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, colors);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData, colors);

        Utilities::DisplayActors({planeActor, centerActor, toothActor}, colors);
    }

    // TODO: it is necessary to consider separately the lower and separately the upper jaw
    void Split_And_CutTooths() {
        constexpr std::string_view fileName {
            R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> separatedData =
                ReadTeethObj(fileName);

        for (const auto& [id, polyData]: separatedData)
        {
            vtkNew<vtkPlane> plane;
            plane->SetOrigin(polyData->GetCenter());

            // TODO: Angle of the plane
            plane->SetNormal(0, 1, 0.1);

            Types::Point<3, double> center {};
            std::array<double, 6> bounds;
            polyData->GetBounds(bounds.data());
            polyData->GetCenter(center.data());

            const Types::Point<3, double> minBound {bounds[0], bounds[2], bounds[4]};
            const Types::Point<3, double> maxBound {bounds[1], bounds[3], bounds[5]};

            double from {0}, until {0};
            if (Utilities::isLowerTooth(id)) {
                from = 0;
                until = maxBound[1] - center[1];;
            }
            else {
                from = minBound[1] - center[1];
                until = 0;;
            }

            // Create cutter
            vtkNew<vtkCutter> cutter;
            cutter->SetCutFunction(plane);
            cutter->SetInputData(polyData);

            cutter->GenerateValues(60, from, until);
            vtkNew<vtkPolyDataMapper> cutterMapper;
            cutterMapper->SetInputConnection(cutter->GetOutputPort());
            cutterMapper->ScalarVisibilityOff();

            vtkNew<vtkActor> planeActor;
            planeActor->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
            planeActor->GetProperty()->SetLineWidth(3);
            planeActor->SetMapper(cutterMapper);

            const vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, colors);
            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData, colors);

            Utilities::DisplayActors({planeActor, centerActor, toothActor}, colors);
        }
    }
}

namespace Experiments::Tests {

    void TestPolyDataNormals() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_1.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkSmartPointer<vtkPolyDataNormals> normals  {vtkPolyDataNormals::New()};

        normals->SetInputData(polyData);
        normals->SetFeatureAngle(80);

        normals->ConsistencyOff();
        normals->SplittingOff();
        normals->ComputeCellNormalsOff();

        normals->AutoOrientNormalsOn();
        normals->UpdateInformation();
        normals->Update();

        Utilities::visualize(polyData);
        Utilities::visualize(normals->GetOutput());
    }

    void BuildPolyBallSurface() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_1.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkSmartPointer<vtkContourFilter> contour  {vtkContourFilter::New()};
        contour->SetInputData(polyData);
        // contour->SetValue(0,0.);
        contour->Update();

        Utilities::visualize(polyData);
        Utilities::visualize(contour->GetOutput());
    }
}

namespace Experiments::Splitter
{
    void SplitSphereWithPlane()
    {
        const vtkSmartPointer<vtkSphereSource> sphere { vtkSmartPointer<vtkSphereSource>::New() };
        sphere->SetRadius(6);
        sphere->SetCenter(-8,0,-14);
        sphere->Update();
        const vtkSmartPointer<vtkPolyData> spherePolyData = sphere->GetOutput();

        Types::Point<3, double> center;
        sphere->GetCenter(center.data());

        const vtkSmartPointer<vtkPlane> plane { vtkSmartPointer<vtkPlane>::New() };
        plane->SetNormal(-1.0, 0.0, 0.0);
        // plane->SetNormal(0.0, 1.0, 0.0);
        plane->SetOrigin(center.data());

        vtkSmartPointer<vtkClipPolyData> clip  { vtkSmartPointer<vtkClipPolyData>::New()};
        clip->SetInputData(spherePolyData);
        clip->SetValue(0.0);
        clip->GenerateClippedOutputOn();
        clip->SetClipFunction (plane);
        clip->Update();

        vtkSmartPointer<vtkPolyData> resultData = clip->GetOutput();
        auto cuttedActor = Utilities::getPolyDataActor(resultData, colors);
        cuttedActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());

        Utilities::DisplayActors({ cuttedActor}, colors);
    }


    void SplitToothWithPlane()
    {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_4.stl)" };
        const vtkSmartPointer<vtkPolyData> toothPolyData = Utilities::readStl(filePath);

        Types::Point<3, double> center;
        toothPolyData->GetCenter(center.data());

        center[1] += 4;

        const vtkSmartPointer<vtkPlane> plane { vtkSmartPointer<vtkPlane>::New() };
        plane->SetNormal(0.0, 0.8, 0.0);
        plane->SetOrigin(center.data());

        vtkSmartPointer<vtkClipPolyData> clip  { vtkSmartPointer<vtkClipPolyData>::New()};
        clip->SetInputData(toothPolyData);
        clip->SetValue(0.0);
        clip->GenerateClippedOutputOn();
        clip->SetClipFunction (plane);
        clip->Update();

        auto toothActor = Utilities::getPolyDataActor(toothPolyData, colors);
        toothActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Green").GetData());

        vtkSmartPointer<vtkPolyData> resultData = clip->GetOutput();
        auto cuttedActor = Utilities::getPolyDataActor(resultData, colors);
        cuttedActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());

        // Utilities::DisplayActors({ cuttedActor}, colors);

        Utilities::visualize(resultData, true);
    }
}

void Experiments::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    // Experiments::vtkMatrix4x4_Tests();

    // Experiments::Split_Mesh_STL();
    // Experiments::Split_Mesh_OBJ();
    // Experiments::Get_Number_Of_Extracted_Regions();


    // Experiments::Show_STL();
    // Experiments::SeparateComponentsTests();
    // Experiments::SeparateComponentsTests2();
    // Experiments::SeparateComponentsTests_Parallel();
    // Experiments::SeparateComponentsTests_ShowTwoToothAsOnePolyData();
    // Experiments::SeparatedComponents_FindClosetPoints();
    // Experiments::ConnectivityAnalysis();


    // Experiments::Transform_Single_Tooth();
    // Experiments::Transform_Single_Tooth_MatrixManual();
    // Experiments::Transform_Single_Tooth_MatrixManual_OBJ_CrownsScan();
    // Experiments::Transform_Single_Tooth_MatrixManual_OBJ_CrownsScan_ManualMatrix();
    // Experiments::Transform_Crowns_STL();

    // OBBTreeTests::VisualizeOBBTree();

    // BoundsAndBorders::Draw_BoundingBox();
    // BoundsAndBorders::Draw_BoundingBox_UsingPoints();
    // BoundsAndBorders::Split_And_TryDrawBoundingBox();
    // BoundsAndBorders::Cutter_Tests_OLD();

    // CovnexHull::SingleTooth_ConvexHull_Test();

    // Holes::SingleTooth_FillHolesTest();

    // LinearInterpolation::Interpolate_Points();

    // DrawText::ShowText();
    // DrawText::ShowText2();
    // DrawText::DrawLine_And_ShowText();
    // DrawText::DrawLineWithTextSizes();

    // Lines::DrawLine();

    // MoveTeeth::OriginPoints_TransformPoints();
    // MoveTeeth::TryMoveTooth(params);

    // Edges::ExtractEdges();
    // Edges::ExtractEdges_Tooth();
    // Edges::ExtractFeatureEdges_Tooth();

    // Cutters::SimpleTest1();
    // Cutters::CutToothWithPlane();
    // Cutters::Cut_Tooth_Vertical();
    // Cutters::Split_And_CutTooths();

    // Tests::TestPolyDataNormals();
    // Tests::BuildPolyBallSurface();

    // Isosurface::Test(); // BAD - Has errors

    // Splitter::SplitToothWithPlane();
    // Splitter::SplitSphereWithPlane();

    // UserInteraction::InteractionTest();
}