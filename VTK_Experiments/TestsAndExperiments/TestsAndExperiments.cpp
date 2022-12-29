//============================================================================
// Name        : TestsAndExperiments.cpp
// Created on  : 25.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TestsAndExperiments
//============================================================================


#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "TestsAndExperiments.h"

#include <iostream>
#include <algorithm>
#include <string_view>
#include <numeric>
#include <random>

namespace TestsAndExperiments {
    vtkSmartPointer<vtkNamedColors> colors{vtkNamedColors::New()};

    vtkSmartPointer<vtkPolyData> transformData(const vtkSmartPointer<vtkPolyData> polyData) {
        vtkSmartPointer<vtkTransform> transform{vtkTransform::New()};
        transform->RotateX(180);
        transform->RotateY(180);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter{vtkTransformPolyDataFilter::New()};
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }

    void ReadFile_AndDisplay() {
        constexpr std::string_view filePath{R"(/home/andtokm/tmp/scan_crown.obj)"};

        vtkSmartPointer<vtkOBJReader> reader{vtkOBJReader::New()};
        reader->SetFileName(filePath.data());
        reader->Update();

        auto data = reader->GetOutput();
        vtkSmartPointer<vtkActor> actor = Utilities::getPolyDataActor(data);

        actor->GetProperty()->SetColor(std::array<double, 3>{0, 1, 1}.data());

        Utilities::DisplayActors({actor});
    }
}

namespace TestsAndExperiments::UnstructuredGrids {

    vtkSmartPointer<vtkUnstructuredGrid> MakeHexahedron()
    {
        // A regular hexagon (cube) with all faces square and three squares around
        // each vertex is created below.

        // Setup the coordinates of eight points
        // (the two faces must be in counter clockwise
        // order as viewed from the outside).

        int numberOfVertices = 8;

        // Create the points
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->InsertNextPoint(0.0, 0.0, 0.0);
        points->InsertNextPoint(1.0, 0.0, 0.0);
        points->InsertNextPoint(1.0, 1.0, 0.0);
        points->InsertNextPoint(0.0, 1.0, 0.0);
        points->InsertNextPoint(0.0, 0.0, 1.0);
        points->InsertNextPoint(1.0, 0.0, 1.0);
        points->InsertNextPoint(1.0, 1.0, 1.0);
        points->InsertNextPoint(0.0, 1.0, 1.0);

        // Create a hexahedron from the points
        vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
        for (int i = 0; i < numberOfVertices; ++i)
        {
            hex->GetPointIds()->SetId(i, i);
        }

        // Add the points and hexahedron to an unstructured grid
        vtkSmartPointer<vtkUnstructuredGrid> uGrid =
                vtkSmartPointer<vtkUnstructuredGrid>::New();
        uGrid->SetPoints(points);
        uGrid->InsertNextCell(hex->GetCellType(), hex->GetPointIds());

        return uGrid;
    }

    vtkSmartPointer<vtkUnstructuredGrid> MakeHexagonalPrism()
    {
        vtkSmartPointer<vtkHexagonalPrism> hexagonalPrism = vtkSmartPointer<vtkHexagonalPrism>::New();
        hexagonalPrism->GetPointIds()->SetId(0, 0);
        hexagonalPrism->GetPointIds()->SetId(1, 1);
        hexagonalPrism->GetPointIds()->SetId(2, 2);
        hexagonalPrism->GetPointIds()->SetId(3, 3);
        hexagonalPrism->GetPointIds()->SetId(4, 4);
        hexagonalPrism->GetPointIds()->SetId(5, 5);
        hexagonalPrism->GetPointIds()->SetId(6, 6);
        hexagonalPrism->GetPointIds()->SetId(7, 7);
        hexagonalPrism->GetPointIds()->SetId(8, 8);
        hexagonalPrism->GetPointIds()->SetId(9, 9);
        hexagonalPrism->GetPointIds()->SetId(10, 10);
        hexagonalPrism->GetPointIds()->SetId(11, 11);

        double scale = 2.0;
        hexagonalPrism->GetPoints()->SetPoint(0, 11 / scale, 10 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(1, 13 / scale, 10 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(2, 14 / scale, 12 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(3, 13 / scale, 14 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(4, 11 / scale, 14 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(5, 10 / scale, 12 / scale, 10 / scale);
        hexagonalPrism->GetPoints()->SetPoint(6, 11 / scale, 10 / scale, 14 / scale);
        hexagonalPrism->GetPoints()->SetPoint(7, 13 / scale, 10 / scale, 14 / scale);
        hexagonalPrism->GetPoints()->SetPoint(8, 14 / scale, 12 / scale, 14 / scale);
        hexagonalPrism->GetPoints()->SetPoint(9, 13 / scale, 14 / scale, 14 / scale);
        hexagonalPrism->GetPoints()->SetPoint(10, 11 / scale, 14 / scale, 14 / scale);
        hexagonalPrism->GetPoints()->SetPoint(11, 10 / scale, 12 / scale, 14 / scale);

        vtkSmartPointer<vtkUnstructuredGrid> ug =vtkSmartPointer<vtkUnstructuredGrid>::New();
        ug->SetPoints(hexagonalPrism->GetPoints());
        ug->InsertNextCell(hexagonalPrism->GetCellType(), hexagonalPrism->GetPointIds());
        return ug;
    }

    vtkSmartPointer<vtkUnstructuredGrid> MakePentagonalPrism()
    {
        vtkSmartPointer<vtkPentagonalPrism> pentagonalPrism = vtkSmartPointer<vtkPentagonalPrism>::New();

        pentagonalPrism->GetPointIds()->SetId(0, 0);
        pentagonalPrism->GetPointIds()->SetId(1, 1);
        pentagonalPrism->GetPointIds()->SetId(2, 2);
        pentagonalPrism->GetPointIds()->SetId(3, 3);
        pentagonalPrism->GetPointIds()->SetId(4, 4);
        pentagonalPrism->GetPointIds()->SetId(5, 5);
        pentagonalPrism->GetPointIds()->SetId(6, 6);
        pentagonalPrism->GetPointIds()->SetId(7, 7);
        pentagonalPrism->GetPointIds()->SetId(8, 8);
        pentagonalPrism->GetPointIds()->SetId(9, 9);

        double scale = 2.0;
        pentagonalPrism->GetPoints()->SetPoint(0, 11 / scale, 10 / scale, 10 / scale);
        pentagonalPrism->GetPoints()->SetPoint(1, 13 / scale, 10 / scale, 10 / scale);
        pentagonalPrism->GetPoints()->SetPoint(2, 14 / scale, 12 / scale, 10 / scale);
        pentagonalPrism->GetPoints()->SetPoint(3, 12 / scale, 14 / scale, 10 / scale);
        pentagonalPrism->GetPoints()->SetPoint(4, 10 / scale, 12 / scale, 10 / scale);
        pentagonalPrism->GetPoints()->SetPoint(5, 11 / scale, 10 / scale, 14 / scale);
        pentagonalPrism->GetPoints()->SetPoint(6, 13 / scale, 10 / scale, 14 / scale);
        pentagonalPrism->GetPoints()->SetPoint(7, 14 / scale, 12 / scale, 14 / scale);
        pentagonalPrism->GetPoints()->SetPoint(8, 12 / scale, 14 / scale, 14 / scale);
        pentagonalPrism->GetPoints()->SetPoint(9, 10 / scale, 12 / scale, 14 / scale);

        vtkSmartPointer<vtkUnstructuredGrid> ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
        ug->SetPoints(pentagonalPrism->GetPoints());
        ug->InsertNextCell(pentagonalPrism->GetCellType(), pentagonalPrism->GetPointIds());
        return ug;
    }

    void DisplayData() {
        // vtkSmartPointer<vtkUnstructuredGrid> data = MakeHexahedron();
        // vtkSmartPointer<vtkUnstructuredGrid> data = MakeHexagonalPrism();
        vtkSmartPointer<vtkUnstructuredGrid> data = MakePentagonalPrism();

        vtkSmartPointer<vtkDataSetMapper> mapper { vtkDataSetMapper::New() };
        mapper->SetInputData(data);

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
        actor->GetProperty()->EdgeVisibilityOn();
        actor->GetProperty()->SetLineWidth(3);
        actor->GetProperty()->SetOpacity(.5);

        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());
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
}

namespace TestsAndExperiments::ParametricObjects
{
    // Callbacks for the interactions
    class SliderCallbackN1 : public vtkCommand {
    public:
        static SliderCallbackN1* New() {
            return new SliderCallbackN1;
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*) {
            vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
            double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
            this->SuperEllipsoid->SetN1(value);
        }

        SliderCallbackN1() : SuperEllipsoid(0) {
        }

        vtkParametricSuperEllipsoid* SuperEllipsoid;
    };

    class SliderCallbackN2 : public vtkCommand {
    public:
        static SliderCallbackN2* New() {
            return new SliderCallbackN2;
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*) {
            vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
            double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
            this->SuperEllipsoid->SetN2(value);
        }

        SliderCallbackN2() : SuperEllipsoid(0) {
        }

        vtkParametricSuperEllipsoid* SuperEllipsoid;
    };


    void DisplatyObjectSlider() {
        vtkNew<vtkNamedColors> colors;

        // Set the background color.
        std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
        colors->SetColor("BkgColor", bkg.data());

        vtkNew<vtkParametricSuperEllipsoid> surface;
        vtkNew<vtkParametricFunctionSource> source;

        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkPolyDataMapper> mapper;
        vtkNew<vtkActor> actor;

        vtkNew<vtkProperty> backProperty;
        backProperty->SetColor(colors->GetColor3d("Tomato").GetData());

        // Create a parametric function source, renderer, mapper, and actor
        source->SetParametricFunction(surface);

        mapper->SetInputConnection(source->GetOutputPort());

        actor->SetMapper(mapper);
        actor->SetBackfaceProperty(backProperty);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
        actor->GetProperty()->SetSpecular(.5);
        actor->GetProperty()->SetSpecularPower(20);

        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->SetWindowName("ParametricSuperEllipsoidDemo");
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(640, 480);
        renderer->AddActor(actor);
        renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
        renderer->ResetCamera();
        renderer->GetActiveCamera()->Azimuth(30);
        renderer->GetActiveCamera()->Elevation(-30);
        renderer->GetActiveCamera()->Zoom(0.9);
        renderer->ResetCameraClippingRange();

        vtkNew<vtkRenderWindowInteractor> interactor;
        interactor->SetRenderWindow(renderWindow);

        // Setup a slider widget for each varying parameter
        double tubeWidth(.008);
        double sliderLength(.008);
        double titleHeight(.04);
        double labelHeight(.04);

        vtkNew<vtkSliderRepresentation2D> sliderRepN1;

        sliderRepN1->SetMinimumValue(0.0);
        sliderRepN1->SetMaximumValue(4.0);
        sliderRepN1->SetValue(1.0);
        sliderRepN1->SetTitleText("Z squareness");

        sliderRepN1->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRepN1->GetPoint1Coordinate()->SetValue(.1, .1);
        sliderRepN1->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRepN1->GetPoint2Coordinate()->SetValue(.9, .1);

        sliderRepN1->SetTubeWidth(tubeWidth);
        sliderRepN1->SetSliderLength(sliderLength);
        sliderRepN1->SetTitleHeight(titleHeight);
        sliderRepN1->SetLabelHeight(labelHeight);

        vtkNew<vtkSliderWidget> sliderWidgetN1;
        sliderWidgetN1->SetInteractor(interactor);
        sliderWidgetN1->SetRepresentation(sliderRepN1);
        sliderWidgetN1->SetAnimationModeToAnimate();
        sliderWidgetN1->EnabledOn();

        vtkNew<SliderCallbackN1> callbackN1;
        callbackN1->SuperEllipsoid = surface;

        sliderWidgetN1->AddObserver(vtkCommand::InteractionEvent, callbackN1);

        vtkNew<vtkSliderRepresentation2D> sliderRepN2;

        sliderRepN2->SetMinimumValue(0.0001);
        sliderRepN2->SetMaximumValue(4.0);
        sliderRepN2->SetValue(1.0);
        sliderRepN2->SetTitleText("XY squareness");

        sliderRepN2->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRepN2->GetPoint1Coordinate()->SetValue(.1, .9);
        sliderRepN2->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRepN2->GetPoint2Coordinate()->SetValue(.9, .9);

        sliderRepN2->SetTubeWidth(tubeWidth);
        sliderRepN2->SetSliderLength(sliderLength);
        sliderRepN2->SetTitleHeight(titleHeight);
        sliderRepN2->SetLabelHeight(labelHeight);

        vtkNew<vtkSliderWidget> sliderWidgetN2;
        sliderWidgetN2->SetInteractor(interactor);
        sliderWidgetN2->SetRepresentation(sliderRepN2);
        sliderWidgetN2->SetAnimationModeToAnimate();
        sliderWidgetN2->EnabledOn();

        vtkNew<SliderCallbackN2> callbackN2;
        callbackN2->SuperEllipsoid = surface;

        sliderWidgetN2->AddObserver(vtkCommand::InteractionEvent, callbackN2);

        vtkNew<vtkSliderRepresentation2D> sliderRepMinimumV;

        sliderRepN1->SetMinimumValue(.0001);
        sliderRepMinimumV->SetMaximumValue(.9999 * vtkMath::Pi());
        sliderRepMinimumV->SetValue(.0001);
        sliderRepMinimumV->SetTitleText("V min");

        sliderRepMinimumV->GetPoint1Coordinate()
                ->SetCoordinateSystemToNormalizedDisplay();
        sliderRepMinimumV->GetPoint1Coordinate()->SetValue(.1, .1);
        sliderRepMinimumV->GetPoint2Coordinate()
                ->SetCoordinateSystemToNormalizedDisplay();
        sliderRepMinimumV->GetPoint2Coordinate()->SetValue(.1, .9);

        sliderRepMinimumV->SetTubeWidth(tubeWidth);
        sliderRepMinimumV->SetSliderLength(sliderLength);
        sliderRepMinimumV->SetTitleHeight(titleHeight);
        sliderRepMinimumV->SetLabelHeight(labelHeight);

        surface->SetN1(0.6);
        surface->SetN2(1.0);

        renderer->ResetCamera();
        renderer->GetActiveCamera()->Azimuth(30);
        renderer->GetActiveCamera()->Elevation(-30);
        renderer->GetActiveCamera()->Zoom(0.9);
        renderer->ResetCameraClippingRange();
        renderWindow->Render();

        interactor->Initialize();

        interactor->Start();
    }

    void DisplayObject() {
        vtkNew<vtkNamedColors> colors;
        colors->SetColor("BkgColor", std::array<unsigned char, 4>{{26, 51, 102, 255}}.data());

        vtkNew<vtkParametricSuperEllipsoid> surface;
        surface->SetN1(0.7);
        surface->SetN2(0.8);
        surface->SetXRadius(1.5);
        surface->SetYRadius(1.6);

        vtkNew<vtkParametricFunctionSource> source;
        source->SetParametricFunction(surface);

        vtkNew<vtkProperty> backProperty;
        backProperty->SetColor(colors->GetColor3d("Tomato").GetData());

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(source->GetOutputPort());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->SetBackfaceProperty(backProperty);
        actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
        actor->GetProperty()->SetSpecular(.5);
        actor->GetProperty()->SetSpecularPower(20);

        vtkNew<vtkRenderer> renderer;
        renderer->AddActor(actor);
        renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
        renderer->ResetCamera();
        renderer->GetActiveCamera()->Azimuth(30);
        renderer->GetActiveCamera()->Elevation(-30);
        renderer->GetActiveCamera()->Zoom(0.9);
        renderer->ResetCameraClippingRange();

        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->SetWindowName("ParametricSuperEllipsoidDemo");
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(640, 480);

        vtkNew<vtkRenderWindowInteractor> interactor;
        interactor->SetRenderWindow(renderWindow);


        renderer->ResetCamera();
        renderer->GetActiveCamera()->Azimuth(30);
        renderer->GetActiveCamera()->Elevation(-30);
        renderer->GetActiveCamera()->Zoom(0.9);
        renderer->ResetCameraClippingRange();
        renderWindow->Render();

        interactor->Initialize();
        interactor->Start();
    }
}

namespace TestsAndExperiments::GeometricFigures {

    double getRandom(const double start = 0.0f,
                     const double end = 10.0f) {
        std::random_device rd{};
        auto generator = std::mt19937 { rd() };
        auto distribution = std::uniform_int_distribution<>(start, end);
        return distribution(generator);
    }

    void Parabola()
    {
        struct Coefficient {
            double a {};
            double b {};
            double c {};
        };

        auto equationParabola = [](double x, const Coefficient& coef) {
            int sign = x > 0 ? 1 : - 1;
            return coef.a * (x * x) + coef.b * x * sign+ coef.c;
        };


        const Coefficient coef { 0.2, 0.01, 0};
        std::vector<double> x {-8, -7, -6, -5, -4, -3, -1.2, 1.2, 3, 4, 5, 6, 7 ,8};

        std::vector<double> y;
        for (const auto v: x)
            y.push_back(equationParabola(v, coef));

        std::vector<Eigen::Vector3d> pts;
        for (size_t i = 0, size = x.size(); i < size; i++) {
            pts.push_back({x[i], y[i], 0});
            std::cout << x[i] << ", " << y[i] << std::endl;
        }

        const auto actor = Utilities::getPointsActor(pts, {1, 0, 0});
        actor->GetProperty()->SetPointSize(12);

        Utilities::DisplayActors({actor});
    }
}

namespace TestsAndExperiments::ToothPreprocessing
{
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

    void Visualize_Tooth_OBJ_Data()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2280/models/crowns.obj)");

        const vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& [id, data] : toothMap)
            if (Utilities::isLowerTooth(id))
                dataAppender->AddInputData(data);
        dataAppender->Update();

        Utilities::visualize(dataAppender->GetOutput());
    }

    void PrintDataMetrics(vtkSmartPointer<vtkPolyData> data, std::string_view text) {
        const auto featureEdges = vtkSmartPointer<vtkFeatureEdges>::New();
        featureEdges->SetInputData(data);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOff();
        featureEdges->NonManifoldEdgesOff();
        featureEdges->ManifoldEdgesOff();
        featureEdges->Update();

        std::cout << "============================== " << text << " =====================" << std::endl;
        std::cout << "Data NumberOfPolys: " << data->GetNumberOfPolys()<< std::endl;
        std::cout << "Data NumberOfLines: " << data->GetNumberOfLines()<< std::endl;
        std::cout << "Data NumberOfCells: " << data->GetNumberOfCells()<< std::endl;
        std::cout << "Data NumberOfVerts: " << data->GetNumberOfVerts()<< std::endl;
        std::cout << "Data NumberOfPoints: " << data->GetNumberOfPoints()<< std::endl;
        std::cout << "NumberOfCells : " << featureEdges->GetOutput()->GetNumberOfCells() << std::endl;
        std::cout << "NumberOfPoints: " << featureEdges->GetOutput()->GetNumberOfPoints() << std::endl;
        std::cout << "NumberOfPolys : " << featureEdges->GetOutput()->GetNumberOfPolys() << std::endl;
        std::cout << "NumberOfLines : " << featureEdges->GetOutput()->GetNumberOfLines() << std::endl;
    }

    void FillHoles_ExtractEdge_Decimate()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2280/models/crowns.obj)");

        constexpr unsigned short toothId { 32 };
        if (auto iter = toothMap.find(toothId); toothMap.end() != iter) {
            vtkSmartPointer<vtkPolyData> toothData = iter->second;
            size_t numberOfOpenEdges { std::numeric_limits<size_t>::max()};

            const auto fillHolesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
            fillHolesFilter->SetInputData(toothData);
            fillHolesFilter->SetHoleSize(1000.0);//1000.0
            fillHolesFilter->Update();
            toothData = fillHolesFilter->GetOutput();

            PrintDataMetrics(toothData, "original");
            Utilities::visualize(toothData);

            {
                const vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter {vtkSmoothPolyDataFilter::New()};
                smoothFilter->SetInputData(toothData);
                smoothFilter->SetNumberOfIterations(1);
                smoothFilter->SetRelaxationFactor(0.4); // 0.01 ?
                smoothFilter->FeatureEdgeSmoothingOff();
                smoothFilter->BoundarySmoothingOn();
                smoothFilter->Update();
                toothData = smoothFilter->GetOutput();

                PrintDataMetrics(toothData, "after vtkSmoothPolyDataFilter");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkTriangleFilter> triangleFilter{vtkTriangleFilter::New()};
                triangleFilter->SetInputData(toothData);
                triangleFilter->Update();
                toothData = triangleFilter->GetOutput();

                PrintDataMetrics(toothData, "after vtkTriangleFilter");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkQuadricDecimation::New()};
                decimation->SetInputData(toothData);
                decimation->SetTargetReduction(0.9);
                decimation->Update();
                toothData = decimation->GetOutput();

                PrintDataMetrics(toothData, "after vtkQuadricDecimation");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkPolyDataNormals> normals{vtkPolyDataNormals::New()};
                normals->SetInputData(toothData);
                normals->ComputePointNormalsOn();
                normals->SplittingOff();
                normals->ConsistencyOn();
                normals->AutoOrientNormalsOff();
                normals->Update();
                toothData = normals->GetOutput();

                PrintDataMetrics(toothData, "after vtkPolyDataNormals");
                Utilities::visualize(toothData);
            }



            {
                const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkQuadricDecimation::New()};
                decimation->SetInputData(toothData);
                decimation->SetTargetReduction(0.5);
                decimation->Update();
                toothData = decimation->GetOutput();

                PrintDataMetrics(toothData, "after vtkQuadricDecimation");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkPolyDataNormals> normals{vtkPolyDataNormals::New()};
                normals->SetInputData(toothData);
                normals->ComputePointNormalsOn();
                normals->SplittingOff();
                normals->ConsistencyOn();
                normals->AutoOrientNormalsOff();
                normals->Update();
                toothData = normals->GetOutput();

                PrintDataMetrics(toothData, "after vtkPolyDataNormals");
                Utilities::visualize(toothData);
            }
        }
    }
}

void TestsAndExperiments::TestAll()
{
    // ReadFile_AndDisplay();
    // UnstructuredGrids::DisplayData();

    // ParametricObjects::DisplayObject();
    // ParametricObjects::DisplatyObjectSlider();

    // GeometricFigures::Parabola();

    // ToothPreprocessing::Visualize_Tooth_OBJ_Data();
    ToothPreprocessing::FillHoles_ExtractEdge_Decimate();
};
