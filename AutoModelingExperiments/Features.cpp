//============================================================================
// Name        : Features.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tooths features
//============================================================================

#include <vtkCutter.h>
#include <vtkIdTypeArray.h>
#include <vtkStripper.h>
#include <vtkSmoothPolyDataFilter.h>
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
#include <vtkSmoothPolyDataFilter.h>
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
#include <vtkPlane.h>
#include <vtkCenterOfMass.h>
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


#include "Utilities.h"
#include "Types.h"
#include "Features.h"

#include <iostream>
#include <iomanip>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace {
    constexpr std::string_view FEATURES_JSON_PATH {
        R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/features_2878.json)"
    };
    constexpr std::string_view CONTACT_POINTS_JSON_FILE {
        R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/contact_points_2878.json)"
    };
    constexpr std::string_view LOWER_STL_FILE_2878 {
        R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_lower.stl)"
    };
}

namespace Features
{
    template<typename  _Ty>
    std::ostream& operator<<(std::ostream& ostream, const std::list<_Ty>& list) {
        for (auto &i : list)
            ostream << i << " ";
        return ostream;
    }

    template<typename  _Ty>
    std::ostream& operator<<(std::ostream& ostream, const std::vector<_Ty>& list) {
        for (auto &i : list)
            ostream << i << " ";
        return ostream;
    }

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data) {
        for (const auto& v: data)
            std::cout << v << " ";
        return stream;
    }

    vtkSmartPointer<vtkActor> getPolyDataActor(std::string_view filePath,
                                              [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData( Utilities::readStl(filePath));

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        // actor->GetProperty()->SetDiffuse(0.8);
        // actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        // actor->GetProperty()->SetSpecular(0.3);
        // actor->GetProperty()->SetSpecularPower(60.0);
        return actor;
    }

    // TODO: Local colors
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New()};
}

namespace Features::Parsing
{
    constexpr std::string_view HILL_POINTS_PARAM { "hill_points" };
    constexpr std::string_view CUTTING_EDGE_PARAM { "cutting_edge" };
    constexpr std::string_view FISSURES_PARAM { "fissures" };

    using Point3d = Types::Point<3, double>;
    using Points  = std::vector<Point3d>;

    struct ToothFeatures {
        Points hillPoints;
        Points cuttingEdge;
        std::vector<Points> fissures;
    };

    struct Features {
        std::map<int16_t, ToothFeatures> teethFeatures;
    };

    //======================================================================

    // TODO: Move to parsers library
    template<typename _Ty>
    std::vector<_Ty> extractList(const boost::property_tree::ptree& node) {
        std::vector<_Ty> result;
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
        return result;
    }

    // TODO: Move to parsers library
    template<typename _Ty>
    void extractList(const boost::property_tree::ptree& node, std::vector<_Ty>& result) {
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
    }

    void parseHillPoints(const boost::property_tree::ptree& node, Points& points) {
        for (const auto& [_, pts] : node.get_child("")) {
            points.emplace_back().assign(extractList<double>(pts));
        }
    }

    void parseFissures(const boost::property_tree::ptree& node, std::vector<Points>& fissures) {
        for (const auto& [_, fissureBlock] : node.get_child("")) {
            Points& fiss = fissures.emplace_back();
            for (const auto& [_, fissures] : fissureBlock.get_child(""))
                fiss.emplace_back().assign(extractList<double>(fissures));
        }
    }

    void parseCuttingEdge(const boost::property_tree::ptree& node, Points& edges) {
        for (const auto& [_, pts] : node.get_child("")) {
            edges.emplace_back().assign(extractList<double>(pts));
        }
    }

    [[nodiscard]] // TODO: check for RNVO -- Performance reasons
    Features Parse() {
        boost::property_tree::ptree root;
        boost::property_tree::read_json(FEATURES_JSON_PATH.data(), root);

        Features features;
        for (const auto& [name, toothData] : root.get_child("")) {
            const auto [iter, ok] = features.teethFeatures.emplace(atoi(name.data()), ToothFeatures {});
            for (const auto& [featName, toothData] : toothData.get_child("")) {
                if (0 == featName.compare(HILL_POINTS_PARAM))
                    parseHillPoints(toothData, iter->second.hillPoints);
                else if (0 == featName.compare(FISSURES_PARAM))
                    parseFissures(toothData, iter->second.fissures);
                else if (0 == featName.compare(CUTTING_EDGE_PARAM))
                    parseCuttingEdge(toothData, iter->second.cuttingEdge);
            }
        }
        return features;
    }
}

namespace FeatureUtils
{
    using Point3f = std::array<float, 3>;
    using Point3d = Types::Point<3, double>;
    using Cell  = std::array<int, 3>;

    // TODO: refactor
    Point3f toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Point3f pt {};
        pt[0] = atof(strPoints.substr(0, pos1).data());
        pt[1] = atof(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atof(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: refactor
    Cell toCell(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Cell pt {};
        pt[0] = atoi(strPoints.substr(0, pos1).data());
        pt[1] = atoi(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atoi(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    [[nodiscard]]
    std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>
    ReadTeethObj(std::string_view path)
    {
        std::ifstream input(path.data());
        std::vector<std::string> lines;
        while(std::getline(input, lines.emplace_back())) { /** Read all file lines. **/ }
        input.close();

        const size_t size {lines.size()};
        constexpr size_t APPROX_DATA_BLOCK_SIZE = 32 * 2;

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap;
        std::vector<Point3f> points;
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
                for (const auto& [x, y, z]: points)
                    vtkPoints->InsertNextPoint(x, y, z);

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

    vtkSmartPointer<vtkPolyData> preprocessMesh(vtkSmartPointer<vtkPolyData> polyData)
    {
        // constexpr size_t n_out_poly_num {0};
        constexpr size_t iterCount {5};
        constexpr double relaxationFactor {0.3};

        auto mesh = polyData;
        // std::cout << "Points = " << mesh->GetNumberOfPolys() << std::endl;

        for (int i = 0; i < iterCount; ++i) {
            vtkNew<vtkSmoothPolyDataFilter> smoother;
            smoother->SetInputData(mesh);
            smoother->SetNumberOfIterations(iterCount);
            smoother->SetRelaxationFactor(relaxationFactor);
            smoother->Update();

            mesh = smoother->GetOutput();
            // std::cout << "Points = " << mesh->GetNumberOfPolys() << std::endl;
        }
        return mesh;
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
}

namespace Features {

    class IsoclinesFilter final {
    private:
        constexpr static inline size_t isoclinesNumber { 400 };

    public:
        vtkSmartPointer<vtkPolyData> toothData;
        FeatureUtils::Point3d normals {0.0, 1.0, 0.0};
        FeatureUtils::Point3d centroid {};

    public:
        IsoclinesFilter(vtkSmartPointer<vtkPolyData> mesh): toothData(mesh) {
            const auto data = FeatureUtils::preprocessMesh(toothData);
            this->centroid = FeatureUtils::GetCentroid(data);
            std::cout << "Centroid: " << centroid << std::endl;

            vtkNew<vtkPlane> plane;
            plane->SetOrigin(centroid.data());
            plane->SetNormal(normals.data());

            std::array<double, 6> bounds;
            toothData->GetBounds(bounds.data());

            FeatureUtils::Point3d minBound {bounds[0], bounds[2], bounds[4]};
            FeatureUtils::Point3d maxBound {bounds[1], bounds[3], bounds[5]};

            std::cout << "Bounds  : " << bounds << std::endl;
            std::cout << "minBound: " << minBound << std::endl;
            std::cout << "maxBound: " << maxBound << std::endl;

            const double minDistZ = centroid[1] - minBound[1];
            const double maxDistZ = maxBound[1] - centroid[1];

            std::cout << "maxDistZ: " << maxDistZ << std::endl;
            std::cout << "minDistZ: " << minDistZ << std::endl;


            vtkNew<vtkCutter> cutter;
            cutter->SetCutFunction(plane);
            cutter->SetInputData(this->toothData);
            cutter->GenerateValues(isoclinesNumber + 2, -minDistZ, maxDistZ);
            cutter->Update();

            std::cout << "GetNumberOfContours = " << cutter->GetNumberOfContours() << std::endl;

            vtkNew<vtkStripper> contourStripper;
            contourStripper->SetInputConnection(cutter->GetOutputPort());
            contourStripper->JoinContiguousSegmentsOn();
            contourStripper->Update();

            const vtkSmartPointer<vtkPolyData> contourStripperOut = contourStripper->GetOutput();

            const std::vector<vtkIdType> lines = [&contourStripperOut] {
                const vtkIdTypeArray* array { contourStripperOut->GetLines()->GetData() };
                std::vector<vtkIdType> vector (array->GetSize());
                for (size_t i = 0; auto& v: vector)
                    v = array->GetValue(i++);
                return vector;
            } ();



            const size_t linesNumber = contourStripperOut->GetNumberOfLines();
            std::list<std::list<int>> indexLines;
            for (size_t i = 0, index = 0; i < linesNumber; ++i) {
                const auto id = lines[index] + 1;
                const auto iter { lines.cbegin() + index };
                const auto line = indexLines.emplace_back(iter + 1, iter + id);
                index += id;
                // TODO: Delete debug output
                // std::cout << "Inserted size = " << line.size() << std::endl;
            }


            std::cout << "lines size      = " << lines.size() << std::endl;
            std::cout << "Number of lines = " << linesNumber << std::endl;
            std::cout << "indexLines size = " << indexLines.size() << std::endl;

            combineContours(indexLines);

            /*
            const std::vector<vtkIdType> points = [&contourStripperOut] {
                const vtkDataArray* array { contourStripperOut->GetPoints()->GetData() };
                std::vector<vtkIdType> vector (array->GetSize());
                for (size_t i = 0; auto& v: vector)
                    v = array->GetValue(i++);
                return vector;
            } ();
            */

            const vtkSmartPointer<vtkDataArray> points = contourStripperOut->GetPoints()->GetData();

            //
            // std::cout << "linesNumber = " << linesNumber << std::endl;
            // std::cout << "lines = "  << lines->GetSize() << std::endl;
            // lines->Print(std::cout);
            // std::cout << "points = " << points->GetDataSize() << std::endl;
            // points->Print(std::cout);


            /*
            for (const auto i: lines) {
                std::cout << i << std::endl;
            }
             */
        }

        void combineContours(std::list<std::list<int>>& indexLines)
        {
            std::list<int> firstPoints, lasePoints;
            for (const auto& line: indexLines) {
                firstPoints.push_back(line.front());
                lasePoints.push_back(line.back());
            }

            for (auto iter = indexLines.begin(); indexLines.end() != iter; ++iter) {
                std::list<int>& line = indexLines.front();
            }
        }
    };

    //-----------------------------------------------------------------------------------------

    void TestFilter()
    {
        // constexpr std::string_view path_13758 {R"(/home/andtokm/Projects/data/cases/13758/automodeling/out/13758_teeth.obj)"};
        constexpr std::string_view path_2878 {R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)"};


        // TODO: Original OBJ file from models
        // constexpr std::string_view path {R"(/home/andtokm/Projects/data/cases/13758/models/5fd2_scan_crown.obj)"};

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap = FeatureUtils::ReadTeethObj(path_2878);

        const vtkSmartPointer<vtkPolyData> mesh = dataMap[21];
        // Utilities::visualize(dataMap[21]);

        IsoclinesFilter filter(mesh);
    }

    void TestDifferentComponentsBound()
    {
        constexpr std::string_view path_13758 {R"(/home/andtokm/Projects/data/cases/13758/automodeling/out/13758_teeth.obj)"};
        constexpr std::string_view path_2878 {R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)"};

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap = FeatureUtils::ReadTeethObj(path_2878);

        const vtkSmartPointer<vtkPolyData> mesh = dataMap[21];
        // Utilities::visualize(dataMap[21]);

        for (const auto& [id, data]: dataMap) {
            std::array<double, 6> bounds;
            data->GetBounds(bounds.data());
            // TODO: We have same bounds for diff componetns
            std::cout << "xmin: " << bounds[0] << " "<< "xmax: " << bounds[1] << " "
                      << "ymin: " << bounds[2] << " "<< "ymax: " << bounds[3] << " "
                      << "zmin: " << bounds[4] << " "<< "zmax: " << bounds[5] << std::endl;
        }
    }

    void VisualizeFeatures(const Parsing::Features& features) {
        /* std::vector<Types::Point<3, double>> cuttingEdgePoints;
        for (const auto& [id, teethFeatures]: features.teethFeatures) {
            if (Utilities::isLowerTooth(id)) {
                for (const Types::Point<3, double> &pt: teethFeatures.cuttingEdge)
                    cuttingEdgePoints.push_back(pt);
            }
        } */

        /* std::vector<Types::Point<3, double>> hillPointsPoints;
        for (const auto& [id, teethFeatures]: features.teethFeatures) {
            if (Utilities::isLowerTooth(id)) {
                for (const Types::Point<3, double> &pt: teethFeatures.hillPoints)
                    hillPointsPoints.push_back(pt);
            }
        }*/

        /* std::vector<Types::Point<3, double>> fissuresPoints;
        for (const auto& [id, teethFeatures]: features.teethFeatures) {
            if (Utilities::isLowerTooth(id)) {
                for (const auto& fissures: teethFeatures.fissures)
                    for (const Types::Point<3, double> &pt: fissures)
                        fissuresPoints.push_back(pt);
            }
        }*/

        std::vector<Types::Point<3, double>> points;
        for (const auto& [id, teethFeatures]: features.teethFeatures) {
            if (Utilities::isLowerTooth(id)) {
                for (const Types::Point<3, double> &pt: teethFeatures.cuttingEdge)
                    points.push_back(pt);
                for (const auto& fissures: teethFeatures.fissures)
                    for (const Types::Point<3, double> &pt: fissures)
                        points.push_back(pt);
            }
        }

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(LOWER_STL_FILE_2878, colors);
        actorPoint->GetProperty()->SetPointSize(6);

        vtkNew<vtkRenderer> renderer;
        renderer->AddActor(actorPoint);
        renderer->AddActor(actorTeeth);
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());

        vtkNew<vtkRenderWindow> window;
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Teeth and Axis visualization");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    template<typename _Ty>
    std::vector<Types::Point<3, _Ty>> Interpolate(const std::vector<Types::Point<3, _Ty>>& points)
    {
        _Ty xSum{}, ySum{}, x2Sum{}, xySum{};
        for (const auto& pt : points) {
            xSum += pt[0];                // calculate sigma(xi)
            ySum += pt[2];                // calculate sigma(yi)
            x2Sum += std::pow(pt[0], 2); // calculate sigma(x^2i)
            xySum += pt[0] * pt[2];      // calculate sigma(xi*yi)
        }

        const size_t size{ points.size() };
        const _Ty a = (size * xySum - xSum * ySum) / (size * x2Sum - xSum * xSum);  // calculate slope
        const _Ty b = (x2Sum * ySum - xSum * xySum) / (size * x2Sum - xSum * xSum);  // calculate intercept

        std::vector<Types::Point<3, _Ty>> pts(points);
        for (size_t i = 0; i < size; ++i)
            pts[i][2] = (a * points[i][0] + b);
        return  pts;
    }

    void Visualize_CuttingAdges_AsList_OneTooth(const Parsing::Features& features) {
        auto toothFeatures = features.teethFeatures.find(44);
        if (features.teethFeatures.end() == toothFeatures)
            return;

        std::vector<Types::Point<3, double>> points;
        for (const Types::Point<3, double> &pt: toothFeatures->second.cuttingEdge)
            points.push_back(pt);

        std::vector<Types::Point<3, double>> pts = Interpolate(points);
        std::for_each(pts.begin(), pts.end(), [](auto& pt) {
            pt[1] = 5;
        });

        std::sort(pts.begin(), pts.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });
        pts.erase(pts.begin() + 1, pts.end() - 1);
        pts.shrink_to_fit();


        // vtkSmartPointer<vtkActor> actorPointForLine = Utilities::getPointsActor(pts, colors);
        vtkSmartPointer<vtkActor> linePoint = Utilities::getLinesActor_FromPoints(pts, colors);
        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(LOWER_STL_FILE_2878, colors);
        actorPoint->GetProperty()->SetPointSize(6);
        // actorPointForLine->GetProperty()->SetPointSize(6);
        linePoint->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

        vtkNew<vtkRenderer> renderer;
        renderer->AddActor(linePoint);
        renderer->AddActor(actorPoint);
        renderer->AddActor(actorTeeth);
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());

        vtkNew<vtkRenderWindow> window;
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Teeth and Axis visualization");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    void Test_Extract_Fissures_WithCutter() {
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

        vtkSmartPointer<vtkCutter> cutter { vtkCutter::New() };
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);
        cutter->GenerateValues(40, -beginDistZ, maxDistZ);

        vtkSmartPointer<vtkStripper> contourStripper { vtkStripper::New() };
        contourStripper->SetInputConnection(cutter->GetOutputPort());
        contourStripper->JoinContiguousSegmentsOn();
        contourStripper->Update();
        const vtkSmartPointer<vtkPolyData> contourStripperOut = contourStripper->GetOutput();

        const std::vector<vtkIdType> linesPoints = [&contourStripperOut] {
            const vtkIdTypeArray* array { contourStripperOut->GetLines()->GetData() };
            std::vector<vtkIdType> vector (array->GetSize());
            for (size_t i = 0; auto& v: vector)
                v = array->GetValue(i++);
            return vector;
        } ();

        const size_t linesNumber = contourStripperOut->GetNumberOfLines();
        std::vector<std::vector<vtkIdType>> indexLines;
        for (size_t i = 0, index = 0; i < linesNumber; ++i) {
            const auto id = linesPoints[index] + 1;
            const auto iter { linesPoints.cbegin() + index };
            const auto line = indexLines.emplace_back(iter + 1, iter + id);
            index += id;
            // TODO: Delete debug output
            // std::cout << "Inserted size = " << line.size() << std::endl;
        }


        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(contourStripperOut);
        mapper->ScalarVisibilityOff();

        vtkNew<vtkActor> planeActor;
        planeActor->GetProperty()->SetColor(colors->GetColor3d("Deep_pink").GetData());
        planeActor->GetProperty()->SetLineWidth(5);
        planeActor->SetMapper(mapper);

        const vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, colors);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData, colors);

        Utilities::DisplayActors({planeActor, centerActor, toothActor}, colors);
    }
}

void Features::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    // TestFilter();
    // TestDifferentComponentsBound();
    Test_Extract_Fissures_WithCutter();

    const Parsing::Features& features = Parsing::Parse();

    // VisualizeFeatures(features);
    // Visualize_CuttingAdges_AsList_OneTooth(features);
}