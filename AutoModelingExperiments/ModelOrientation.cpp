//============================================================================
// Name        : Points.cpp
// Created on  : 08.10.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include <vtkCutter.h>
#include <vtkLineSource.h>
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
#include "ModelOrientation.h"

#include <iostream>
#include <iomanip>
#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

namespace ModelOrientation {

    /*
    class PointStruct
    {
    public:
        double* data() {
            return point;
        }

    public:
        double point[3];
        PointStruct() {}

        PointStruct(double *pt) {
            point[0]=pt[0];
            point[1]=pt[1];
            point[2]=pt[2];
        }

        PointStruct(double x, double y, double z) {
            point[0]=x;
            point[1]=y;
            point[2]=z;
        }

        void GetPoint(double& x, double& y, double& z) {
            x = point[0];
            y = point[1];
            z = point[2];
        }

        PointStruct & SetPoint(double x, double y, double z) {
            point[0] = x;
            point[1] = y;
            point[2] = z;
            return *this;
        }

        PointStruct & SetPoint(double p[]) {
            point[0]=p[0];
            point[1]=p[1];
            point[2]=p[2];
            return *this;
        }

        PointStruct & Setpoint(float p[]) {
            point[0]=p[0];
            point[1]=p[1];
            point[2]=p[2];
            return *this;
        }

        double &operator[](int i) {
            return point[i];
        }

        double operator[](int i) const {
            return point[i];
        }


        PointStruct operator=(PointStruct p) {
            SetPoint( p.point);
            return *this;
        }

        PointStruct operator+=(const PointStruct &p) {
            point[0] += p[0];
            point[1] += p[1];
            point[2] += p[2];
            return *this;
        }

        PointStruct operator-=(const PointStruct &p) { point[0] -= p[0];    point[1] -= p[1]; point[2] -= p[2]; return *this; }
        PointStruct operator*=(double v) { point[0]*=v; point[1]*=v; point[2]*=v;   return *this; }
        PointStruct operator/=(double v) { point[0]/=v; point[1]/=v; point[2]/=v;   return *this; }
        PointStruct operator^=( PointStruct &p) {
            SetPoint(point[1]*p.point[2]-point[2]*p.point[1], point[2]*p.point[0]-point[0]*p.point[2], point[0]*p.point[1]-point[1]*p.point[0]); return *this; }

        PointStruct operator+(double v) { return PointStruct(point[0]+v, point[1]+v, point[2]+v); }
        PointStruct operator-(double v) { return PointStruct(point[0]-v, point[1]-v, point[2]-v); }
        PointStruct operator*(double v) { return PointStruct(point[0]*v, point[1]*v, point[2]*v); }
        PointStruct operator/(double v) { return PointStruct(point[0]/v, point[1]/v, point[2]/v); }
        PointStruct operator-() { return PointStruct(-point[0], -point[1], -point[2]); }

        double Dot(const PointStruct &p) { return point[0]*p.point[0]+point[1]*p.point[1]+point[2]*p.point[2]; }

        double Length() { return sqrt(point[0]*point[0]+point[1]*point[1]+point[2]*point[2]); }

        PointStruct Unit() {
            std::cout << "LEN = " << Length() << std::endl;
            return (operator/=(Length()));
        }

        friend PointStruct operator*(double v,PointStruct &p) { return p*v; };
        friend PointStruct operator/( PointStruct &p, double v) { return p*(1./v); }
        friend PointStruct operator+(const PointStruct &p1, const PointStruct &p2) { return PointStruct(p1[0]+p2[0],p1[1]+p2[1],p1[2]+p2[2]); }
        friend PointStruct operator-(const PointStruct &p1, const PointStruct &p2) { return PointStruct(p1[0]-p2[0],p1[1]-p2[1],p1[2]-p2[2]); }
        friend double operator*( PointStruct &p1,  PointStruct &p2) { return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2]; }
        friend PointStruct operator^(const PointStruct &p1, const PointStruct &p2) { return PointStruct(p1[1]*p2[2]-p1[2]*p2[1],p1[2]*p2[0]-p1[0]*p2[2],p1[0]*p2[1]-p1[1]*p2[0]); }
        friend bool operator==( PointStruct &p1,  PointStruct &p2)
        {
            bool notEqual = (p1 != p2);
            return !notEqual;
        }
        friend bool operator!=( PointStruct &p1,  PointStruct &p2)
        {
            if( fabs( p1[0] - p2[0] ) > 1e-6 ||
                fabs( p1[1] - p2[1] ) > 1e-6 ||
                fabs( p1[2] - p2[2] ) > 1e-6 )
            {
                return true;
            }
            return false;
        }
        friend bool operator<( PointStruct &p1,  PointStruct &p2) { return (p1[0] <=  p2[0] && p1[1] <= p2[1] && p1[2] <= p2[2]); }
        friend ostream& operator<<(ostream & os, PointStruct & p) { os << "PointStruct [" << p[0] <<", " << p[1] <<", " << p[2] << "]\n"; return os; }
    };
    */


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

    using Point3D = Types::Point<3, double>;

    // TODO: Move this somewhere else
    template<size_t _Size = 2, typename Ty = double >
    void PointUnits(Types::Point<_Size, Ty>& pt) {
        const Ty len { pt.distanceTo({}) };
        pt = pt / len;
    }
}

namespace ModelOrientation {
    const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

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

    void Display_AxisBonds_Test() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/cases/2878/models/a6db_scan_crown.obj)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        using PointType = Point3D;
        // using PointType = PointStruct;

        PointType corner, max, mid, min, size;
        const vtkSmartPointer<vtkOBBTree> obbTree { vtkOBBTree::New()};
        obbTree->ComputeOBB( polyData,
                             corner.data(),
                             max.data(),
                             mid.data(),
                             min.data(),
                             size.data());
        cout << "corner: (" << corner[0] << ", " << corner[1] << ", " << corner[2] << ")\n";
        cout << "max: (" << max[0] << ", " << max[1] << ", " << max[2] << ")\n";
        cout << "mid: (" << mid[0] << ", " << mid[1] << ", " << mid[2] << ")\n";
        cout << "min: (" << min[0] << ", " << min[1] << ", " << min[2] << ")\n";
        cout << "size: (" << size[0] << ", " << size[1] << ", " << size[2] << ")\n";

        const double xSize { max.distanceTo({}) };
        const double ySize { mid.distanceTo({}) };
        const double zSize { min.distanceTo({}) };

        std::cout << "=============================================================================\n";
        std::cout << xSize << "  " << ySize << "  " << zSize << std::endl;
        std::cout << "=============================================================================\n";

        PointUnits(max);
        PointUnits(mid);
        PointUnits(min);

        cout << "\nmax: (" << max[0] << ", " << max[1] << ", " << max[2] << ")\n";
        cout << "mid: (" << mid[0] << ", " << mid[1] << ", " << mid[2] << ")\n";
        cout << "min: (" << min[0] << ", " << min[1] << ", " << min[2] << ")\n";

        cout << "corner: " << corner << std::endl;
        cout << "max   : " << max;
        cout << "mid   : " << mid;
        cout << "min   : " << min ;
        cout << "size  : " << size;

        PointType tmp2(corner + max * size[0]);
        cout << "\ncorner1: " << corner<< std::endl;
        cout << "corner2: " << tmp2<< std::endl;

        const vtkSmartPointer<vtkActor> actorMaxLine = getLineActor({corner, corner + max * xSize}, colors);
        const vtkSmartPointer<vtkActor> actorMidLine = getLineActor({corner, corner + mid * ySize}, colors);
        const vtkSmartPointer<vtkActor> actorMinLine = getLineActor({corner, corner + min * zSize}, colors);
        vtkSmartPointer<vtkActor> actor4 = Utilities::getPolyDataActor(polyData, colors);

        Utilities::DisplayActors({actorMaxLine, actorMidLine, actorMinLine, actor4}, colors);
    }

    void FindAngleAndRotateMesh() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/cases/2878/models/a6db_scan_crown.obj)"};
        vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        Types::Point<3, double> corner, max, mid, min, size;
        const vtkSmartPointer<vtkOBBTree> obbTree { vtkOBBTree::New()};
        obbTree->ComputeOBB(polyData, corner.data(), max.data(), mid.data(), min.data(), size.data());

        double xSize { max.distanceTo({}) };
        double ySize { mid.distanceTo({}) };
        double zSize { min.distanceTo({}) };

        PointUnits(max);
        PointUnits(mid);
        PointUnits(min);

        Types::Point<3, double> pointZ {corner + min * zSize};
        Types::Point<3, double> tmp {corner[0], corner[1] + pointZ.distanceTo(corner), corner[2]};



        // Points to calc angle:
        const Types::Point<3, double> a = tmp - corner, b = pointZ - corner;
        const double x = (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (a.distanceTo({}) * b.distanceTo({}));
        const double angle = acos(x) * 180.0f / M_PI;

        std::cout << "angle = " << angle << std::endl;

        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transform->RotateX(angle);
        transform->RotateY(180);

        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        polyData = transformFilter->GetOutput();

        obbTree->ComputeOBB(polyData, corner.data(), max.data(), mid.data(), min.data(), size.data());

        xSize = max.distanceTo({});
        ySize = mid.distanceTo({});
        zSize = min.distanceTo({});

        PointUnits(max);
        PointUnits(mid);
        PointUnits(min);


        const vtkSmartPointer<vtkActor> actorMaxLine = getLineActor({corner, corner + max * xSize}, colors);
        const vtkSmartPointer<vtkActor> actorMidLine = getLineActor({corner, corner + mid * ySize}, colors);
        const vtkSmartPointer<vtkActor> actorMinLine = getLineActor({corner, corner + min * zSize}, colors);
        // const vtkSmartPointer<vtkActor> actorTargetLine = getLineActor({corner, tmp}, colors);
        /// actorTargetLine->GetProperty()->SetColor( 0.5, 0.3, 0 );

        vtkSmartPointer<vtkActor> actor4 = Utilities::getPolyDataActor(transformFilter->GetOutput(), colors);

        Utilities::DisplayActors({
            actorMaxLine,
            actorMidLine,
            actorMinLine,
            actor4
            },colors);
    }
};

void ModelOrientation::TestAll([[maybe_unused]] const std::vector<std::string_view>& params) {
    // Display_AxisBonds_Test();
    FindAngleAndRotateMesh();
}
