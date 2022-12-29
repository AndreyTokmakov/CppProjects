//============================================================================
// Name        : Features.h
// Created on  : 30.09.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tooths features
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

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
#include <string>
#include <string_view>
#include <vector>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "../Types.h"
#include "ContactPoints.h"
#include "../Utilities.h"

namespace {

    const std::string CASE_ID {"10059"};
    // const std::string TYPE {"/before/"};
    const std::string TYPE {"/after/"};

    const std::string CONTACT_POINTS_JSON_FILE {
        std::string(R"(/home/andtokm/Projects/data/cases_ml/)")
            .append(CASE_ID).append(TYPE).append(R"(contact_points_)")
            .append(CASE_ID).append(".json")
    };

    const std::string LOWER_TEETH_STL_FILE {
        std::string(R"(/home/andtokm/Projects/data/cases_ml/)")
            .append(CASE_ID).append(TYPE).append(R"(/crowns/)")
            .append(CASE_ID).append("_lower.stl")
    };

    const std::string UPPER_TEETH_STL_FILE {
        std::string(R"(/home/andtokm/Projects/data/cases_ml/)")
            .append(CASE_ID).append(TYPE).append(R"(/crowns/)")
            .append(CASE_ID).append("_upper.stl")
    };
}

namespace ContactPoints {

    template<typename _Ty>
    std::vector<_Ty> extractList(const boost::property_tree::ptree& node) {
        std::vector<_Ty> result;
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
        return result;
    }

    template<typename _Ty>
    void extractList(const boost::property_tree::ptree& node, std::vector<_Ty>& result) {
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
    }

    std::vector<Point3d> Points::getUpperPoints() const noexcept {
        std::vector<Point3d> points;
        for (const auto& [id, pt]: teethContactPoints) {
            if (Utilities::isUpperTooth(id)) {
                points.push_back(pt.pt1);
                points.push_back(pt.pt2);
            }
        }
        return points;
    }

    std::vector<Point3d> Points::getLowerPoints() const noexcept {
        std::vector<Point3d> points;
        for (const auto& [id, pt]: teethContactPoints) {
            if (Utilities::isLowerTooth(id)) {
                points.push_back(pt.pt1);
                points.push_back(pt.pt2);
            }
        }
        return points;
    }

    // TODO: check for RNVO
    //       Performance reasons
    Points Parse(std::string_view filePath) {
        boost::property_tree::ptree root;
        boost::property_tree::read_json(filePath.data(), root);

        Points points;
        for (const auto& [name, value] : root.get_child("")) {
            const auto [iter, ok] = points.teethContactPoints.emplace(atoi(name.data()), ToothContactPoints {});
            std::vector<std::vector<double>> pts;
            for (const auto& [_, nodeData] : value.get_child(""))
                extractList<double>(nodeData, pts.emplace_back());
            // TODO: Assume that we have only to points
            iter->second.pt1.assign(pts[0]);
            iter->second.pt2.assign(pts[1]);
        }
        return points;
    }
}

namespace ContactPoints::Tests {

    vtkSmartPointer<vtkActor> getTeethActor(std::string_view filePath,
                                            [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };


        // const vtkSmartPointer<vtkMatrix4x4> mX = MatrixUtils::GetRotationMatrixX(90);
        // const vtkSmartPointer<vtkMatrix4x4> mY = MatrixUtils::GetRotationMatrixY(180);
        // vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        // vtkMatrix4x4::Multiply4x4(mX.Get(), mY.Get(), matrix.Get());
        // transform->SetMatrix(matrix);


        transformFilter->SetInputData(Utilities::readStl(filePath));
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData(transformFilter->GetOutput());

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        //actor->GetProperty()->SetDiffuse(0.8);
        // actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        // actor->GetProperty()->SetSpecular(0.3);
        // actor->GetProperty()->SetSpecularPower(60.0);

        return actor;
    }

    void DrawTeethAndContactPoints_Lower(const Points& contactPoints) {
        vtkNew<vtkNamedColors> colors;

        double xMin = std::numeric_limits<decltype(xMin)>::max();
        double xMax = std::numeric_limits<decltype(xMax)>::min();
        double yMin = std::numeric_limits<decltype(yMin)>::max();
        double yMax = std::numeric_limits<decltype(yMax)>::min();

        for (const auto& [k, pts]: contactPoints.teethContactPoints) {
            xMin = std::min(xMin, pts.pt1[0]);
            xMin = std::min(xMin, pts.pt2[0]);
            xMax = std::max(xMax, pts.pt1[0]);
            xMax = std::max(xMax, pts.pt2[0]);

            yMin = std::min(yMin, pts.pt1[1]);
            yMin = std::min(yMin, pts.pt2[1]);
            yMax = std::max(yMax, pts.pt1[1]);
            yMax = std::max(yMax, pts.pt2[1]);
        }
        const auto xAvg = (std::abs(xMin) + std::abs(xMax)) / 2;
        const auto xTemp = xAvg / 8;  // 1/4 part of the tooth
        const auto yAvg = (yMin + yMax) / 2; // For Lower/Upper teeth selection

        // TODO: Test for lower jaw / teeth
        std::vector<Types::Point<3, double>> points;
        points.reserve(contactPoints.teethContactPoints.size());
        for (const auto& [k, pts]: contactPoints.teethContactPoints) {
            if (pts.pt1[1] < yAvg) // Lower teeth
                points.push_back(pts.pt1);
            if (pts.pt2[1] < yAvg) // Lower teeth
                points.push_back(pts.pt2);
        }

        // TODO: Skip points located to close on the X axe
        std::vector<Types::Point<3, double>> pointsUnique;
        for (const auto& pt: points) {
            const double x = pt[0];
            const bool b = std::any_of(pointsUnique.cbegin(), pointsUnique.cend(), [&](const auto& p) {
                return (p[0] > (x - xTemp)) && (p[0] < (x + xTemp));
            });
            if (false == b)
                pointsUnique.push_back(pt);
        }

        points.swap(pointsUnique);
        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);


        double incisorsDist {0};
        for (size_t i = 1; i < incisors.size(); ++i) {
            const double x = incisors[i - 1][0] - incisors[i][0];
            const double y = incisors[i - 1][1] - incisors[i][1];
            const double z = incisors[i - 1][2] - incisors[i][2];
            incisorsDist += std::hypot(x, y, z);
        }

        std::cout << "incisorsDist" << " = " << incisorsDist<< std::endl;

        const double dist1 = std::abs(points[0][0] - points[points.size() - 1][0]);
        const double dist2 = std::abs(points[1][0] - points[7][0]);

        // std::cout << "dist1" << " = " << dist1<< std::endl;
        std::cout << "dist" << " = " << (dist2 + dist1) / 2 << std::endl;
        std::cout << "dist expected" << " = " << incisorsDist * 10 / 8 << std::endl;

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getTeethActor(LOWER_TEETH_STL_FILE, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }

    void DrawTeethAndContactPoints_Upper(const Points& contactPoints) {
        vtkNew<vtkNamedColors> colors;

        /*

        double xMin = std::numeric_limits<decltype(xMin)>::max();
        double xMax = std::numeric_limits<decltype(xMax)>::min();
        double yMin = std::numeric_limits<decltype(yMin)>::max();
        double yMax = std::numeric_limits<decltype(yMax)>::min();
        double zMin = std::numeric_limits<decltype(zMin)>::max();
        double zMax = std::numeric_limits<decltype(zMax)>::min();

        for (const auto& [k, pts]: contactPoints.teethContactPoints) {
            xMin = std::min(xMin, pts.pt1[0]);
            xMin = std::min(xMin, pts.pt2[0]);
            xMax = std::max(xMax, pts.pt1[0]);
            xMax = std::max(xMax, pts.pt2[0]);

            yMin = std::min(yMin, pts.pt1[1]);
            yMin = std::min(yMin, pts.pt2[1]);
            yMax = std::max(yMax, pts.pt1[1]);
            yMax = std::max(yMax, pts.pt2[1]);

            zMin = std::min(zMin, pts.pt1[2]);
            zMin = std::min(zMin, pts.pt2[2]);
            zMax = std::max(zMax, pts.pt1[2]);
            zMax = std::max(zMax, pts.pt2[2]);
        }

        const auto xAvg = (std::abs(xMin) + std::abs(xMax)) / 2;
        const auto xTemp = xAvg / 8;  // 1/4 part of the tooth
        const auto yAvg = (yMin + yMax) / 2; // For Lower/Upper teeth selection
        const auto zDist = std::abs(zMin + zMax);

        // TODO: Test for lower jaw / teeth
        std::vector<Types::Point<3, double>> points;
        points.reserve(contactPoints.teethContactPoints.size());
        for (const auto& [k, pts]: contactPoints.teethContactPoints) {
            if (pts.pt1[1] > yAvg) // Lower teeth
                points.push_back(pts.pt1);
            if (pts.pt2[1] > yAvg) // Lower teeth
                points.push_back(pts.pt2);
        }


        // TODO: Skip points located to close on the X axe
        std::vector<Types::Point<3, double>> pointsUnique;
        for (const auto& pt: points) {
            const double x = pt[0];
            const bool b = std::any_of(pointsUnique.cbegin(), pointsUnique.cend(), [&](const auto& p) {
                return (p[0] > (x - xTemp)) && (p[0] < (x + xTemp));
            });
            if (false == b)
                pointsUnique.push_back(pt);
        }

        for (const auto& pt: points) {
            const double x = pt[0];
            const bool b = std::any_of(pointsUnique.cbegin(), pointsUnique.cend(), [&](const auto& p) {
                return (p[0] > (x - xTemp)) && (p[0] < (x + xTemp));
            });
            if (false == b)
                pointsUnique.push_back(pt);
        }

        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);


        double incisorsDist {0};
        for (size_t i = 1; i < incisors.size(); ++i) {
            const double x = incisors[i - 1][0] - incisors[i][0];
            const double y = incisors[i - 1][1] - incisors[i][1];
            const double z = incisors[i - 1][2] - incisors[i][2];
            incisorsDist += std::hypot(x, y, z);
        }

        std::cout << "incisorsDist" << " = " << incisorsDist<< std::endl;

        const double dist1 = std::abs(points[0][0] - points[points.size() - 1][0]);
        const double dist2 = std::abs(points[1][0] - points[7][0]);

        // std::cout << "dist1" << " = " << dist1<< std::endl;
        std::cout << "dist" << " = " << (dist2 + dist1) / 2 << std::endl;
        std::cout << "dist expected" << " = " << incisorsDist * 10 / 8<< std::endl;

        */

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(contactPoints.getUpperPoints(), colors);
        vtkSmartPointer<vtkActor> actorTeeth = getTeethActor(UPPER_TEETH_STL_FILE, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }

    void VisualizePointsTests_FindDistToLIne() {
        vtkNew<vtkNamedColors> colors;
        const std::vector<Types::Point<3, double>> linePoints {
                { -6.0, 0.0, -2.0},
                {  2.0, 0.0, -2.0}
        };
        const std::vector<Types::Point<3, double>> points {
               { 0.0, 4.0, 6.0  }
        };

        double distSquread1 = vtkLine::DistanceToLine(points[0].data(),
                                               linePoints[0].data(),
                                               linePoints[1].data());
        double t;
        double closest[3];
        double distSquread2 = vtkLine::DistanceToLine(points[0].data(),
                                               linePoints[0].data(),
                                                linePoints[1].data(), t, closest);

        std::cout << "Dist 1 = " << std::sqrt(distSquread1) << std::endl;
        std::cout << "Dist 2 = " << std::sqrt(distSquread2)  << std::endl;
        std::cout << "t = " << t << std::endl;
        std::cout << "Closest: [" << closest[0] << "  " << closest[1] << "  " << closest[2] << "]" << std::endl;


        /** Find the squared distance between the points. **/
        double squaredDistance = vtkMath::Distance2BetweenPoints(points[0].data(), closest);
        /** Take the square root to get the Euclidean distance between the points **/
        double distance = std::sqrt(squaredDistance);

        /** Output the results. **/
        std::cout << "SquaredDistance = " << squaredDistance << std::endl;
        std::cout << "Distance = " << distance << std::endl;

        /** Add point to targetPoints**/
        std::vector<Types::Point<3, double>> targetPoints;
        targetPoints.push_back({closest[0], closest[1], closest[2]});


        /********************** VISUALIZATION BLOCK: *************************/

        vtkSmartPointer<vtkActor> linesActorPoint = Utilities::getPointsActor(linePoints, colors);
        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> targetPointsActor = Utilities::getPointsActor(targetPoints, colors);

        linesActorPoint->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
        targetPointsActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

        Utilities::DisplayActors({linesActorPoint, linesActorPoint, targetPointsActor}, colors);
    }

    void DrawTeethAndContactPoints_Experiments(const Points& contactPoints) {
        vtkNew<vtkNamedColors> colors;

        std::vector<Types::Point<3, double>> points{
                { -21.37516780566391,  7.71654283503751,  4.309933591970717  },
                { -18.798992897663187, 7.529394716998286, 11.160850739868003 },
                {  17.90494093201055,  7.636524496510928, 11.015867745896811 },
                {  20.24224301008987,  7.583678280072945,  4.18087576873147  },
                { -14.89378671999506,  7.629965018346851, 18.6534126520938   },
                { -9.258607366094143,  6.54667206508766,  22.204924081636303 },
                { -0.4743870439242235, 5.953008265825731, 26.073406571104993 },
                { 14.230078389548419,  7.187540945604041, 18.545068118183483 },
                {  8.838817327018326,  6.694491116069133, 22.953933250029994 }
        };

        std::vector<Types::Point<3, double>> molarsPoints {
                { 24.830078389548419,  6.187540945604041, -10.845068118183483 },
                { -25.230078389548419, 6.187540945604041, -10.845068118183483 },
        };

        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);

        double incisorsDist {0};
        for (size_t i = 1; i < incisors.size(); ++i)
            incisorsDist += incisors[i - 1].distanceTo(incisors[i]);

        std::vector<Types::Point<3, double>> premolars {
                points[0], points[1], points[7], points[8]
        };

        const double dist1 = std::abs(premolars[0][0] - premolars[3][0]) +
                             std::abs(premolars[1][0] - premolars[2][0]);

        // const double dist2 = molarsPoints.front().distanceTo(molarsPoints.back());
        const double dist2 = std::abs(molarsPoints[0][0] - molarsPoints[1][0]);


        std::cout << "incisorsDist              = " << incisorsDist<< std::endl;
        std::cout << "dist (Premolars)          = " << dist1 / 2 << std::endl;
        std::cout << "dist expected (Premolars) = " << (incisorsDist * 100) / 85 << std::endl;
        std::cout << "dist (Molars)             = " << dist2  << std::endl;
        std::cout << "dist expected (Molars)    = " << (incisorsDist * 100) / 65 << std::endl;

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(molarsPoints, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getTeethActor(LOWER_TEETH_STL_FILE, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }
};

void ContactPoints::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    const Points& points = Parse(CONTACT_POINTS_JSON_FILE);

    // Tests::DrawTeethAndContactPoints_Lower(points);
    Tests::DrawTeethAndContactPoints_Upper(points);
    // Tests::DrawTeethAndContactPoints_Experiments(points);
    // Tests::VisualizePointsTests_FindDistToLIne();
}
