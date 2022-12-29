//============================================================================
// Name        : Axis.cpp
// Created on  : 22.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Axis
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <vtkSmoothPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkAxes.h>
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
#include <iomanip>
#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

#include "Types.h"
#include "Utilities.h"
#include "Axis.h"

/* Eigen
#include <Core>
#include <Geometry>
*/

#if 0

namespace Axis::Original {

	template<typename T>
	using aligned_vector = typename std::vector<T, Eigen::aligned_allocator<T>>;
    using array1d = Eigen::Array<double, Eigen::Dynamic, 1>;

	using Vector3d = Eigen::Vector3d;
	using Point3d = Vector3d;

    struct Axis
	{
		int id = 0; //! id teeth
		Point3d origin;	//! origin local
		Vector3d symmetry; //! Y local
		Vector3d angulation; //! X local
    };

    using vector_axis = aligned_vector<Axis>;
    static inline constexpr float _LINEWIDTH {1.0f};
}


namespace Axis::Original {

    void VisPoints(const double *pts,
                   const size_t nPoints,
	               double color_pt[3],
	               vtkSmartPointer<vtkActor> &actorPoints)
    {
        // Create the geometry of a point (the coordinate)
        auto points = vtkSmartPointer<vtkPoints>::New();

        for (size_t i = 0; i < nPoints; ++i) {
            points->InsertNextPoint(pts);
            pts += 3;
        }

        auto pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
        pointsPolydata->SetPoints(points);

        auto vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        // Create a polydata object
        auto polydata = vtkSmartPointer<vtkPolyData>::New();
        polydata->ShallowCopy(vertexFilter->GetOutput());
        // Visualize
        auto mapperPoints = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapperPoints->SetInputData(polydata);
        actorPoints->SetMapper(mapperPoints);
        actorPoints->GetProperty()->SetColor(color_pt);
    }


    bool TeethAxisVis(const vector_axis &axis,
                     ptrdiff_t n_start,
                     ptrdiff_t n_last,
                     const std::vector<unsigned char*> &colors_lines, // 3*(n_last + n_start) or size axis
                     double color_point[3],
                     vtkSmartPointer<vtkActor> &actorLines,
                     vtkSmartPointer<vtkActor> &actorPoints)
    {
        if (axis.empty() || n_start < 0)
            return false;
        const ptrdiff_t nAxis = axis.size();
        if (n_last >= nAxis)
            n_last = nAxis - 1;
        if (n_start > n_last)
            n_start = n_last;

        std::cout << "[DEBUG] " << __FILE__ << "." << __LINE__ << " [" << __FUNCTION__ << "] VisAxis: " << std::endl;
        for (const auto& [id, origin, symmetry, angulation]: axis) {
            std::cout << "	{" << id << std::endl;
            std::cout << "	origin    : [" << std::setprecision(32) << origin.x() << ", " << origin.y() << ", " << origin.z() << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << symmetry.x() << ", " << symmetry.y() << ", " << symmetry.z() << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << angulation.x() << ", " << angulation.y() << ", " << angulation.z() << "]" << std::endl;
        }



        array1d origin;
        const ptrdiff_t nOrigin = n_last - n_start + 1;
        origin.resize(nOrigin * 3);
        for (int i = n_start, j = 0; i <= n_last; ++i, j++) {
            origin[j++] = axis[i].origin.x();
            origin[j++] = axis[i].origin.y();
            origin[j] = axis[i].origin.z();
        }


        /**  Visualize points: **/
        VisPoints(origin.data(), nOrigin, color_point, actorPoints);


        /**  Visualize axis lines: **/
        auto colorsArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colorsArray->SetNumberOfComponents(3);
        colorsArray->SetName("Colors");

        auto points = vtkSmartPointer<vtkPoints>::New();
        const ptrdiff_t nCount = (n_last - n_start + 1);

        Vector3d z_l;
        Point3d pt;
        for (ptrdiff_t j = n_start; j <= n_last; ++j) {
            // Y local
            pt = axis[j].origin - axis[j].symmetry;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + axis[j].symmetry * 20; // 20 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[0]);


            // X local
            pt = axis[j].origin - axis[j].angulation;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + axis[j].angulation * 8; // 8 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[1]);


            // Z local
            z_l = axis[j].angulation.cross(axis[j].symmetry);
            pt = axis[j].origin - z_l;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + z_l * 8; // 8 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[2]);
        }

        // lines
        const ptrdiff_t nPoints = 3 * 2 * nCount;
        auto lines = vtkSmartPointer<vtkCellArray>::New();
        for (int i = 0; i < nPoints; i += 2) {
            auto line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
            lines->InsertNextCell(line);
        }
        auto polyDataLines = vtkSmartPointer<vtkPolyData>::New();
        polyDataLines->SetPoints(points);
        polyDataLines->SetLines(lines);
        polyDataLines->GetCellData()->SetScalars(colorsArray);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(polyDataLines);
        actorLines->GetProperty()->SetLineWidth(_LINEWIDTH);
        actorLines->SetMapper(mapper);

        return true;
    }


    vector_axis getAxisData() {
        vector_axis data;
data.push_back({20,
		{-17.896692992444137360052991425619, -3.9969198251673319077781343366951, 8.8359419949724493648091083741747},
		{0.015708419435684541770603672716788, 0.99211470131447776488187173526967, 0.12434494358242779987744341951839},
		{0.32071189044722075989213294633373, -0.12278889770548938320526843881453, 0.93918409799465885079428062454099}});
data.push_back({21,
		{-13.086340402933384297057273215614, -5.1170216201952669621277891565114, 12.942869366078982196199831378181},
		{0.016073887605727189137327926005128, 0.9381718940605153767009483090078, 0.34579636685792941586115034624527},
		{0.82564303944401362667093735581147, -0.20752101511455056659372075955616, 0.52464140105740553199353826130391}});
data.push_back({22,
		{-7.2028171979120481083214144746307, -3.8829669840949030401588970562443, 16.139603355482666557918491889723},
		{-0.13985067314450275177684090976982, 0.81670376023907576001903407814098, 0.55985422855631261640496632026043},
		{0.98197021122011285410025038800086, 0.041768535088715079417465148026167, 0.18436348269890570628071202463616}});
data.push_back({23,
		{-2.3483565298472459659251398988999, -2.8797274000887140843474298890214, 17.101444922953891847328122821636},
		{-0.03808069627805529822062169387209, 0.69380705819800225775395574601134, 0.71915340961829032906393877055962},
		{0.98109722481512640079870379850036, -0.11068543453301099566132847940025, 0.15873553490726644010777590665384}});
data.push_back({24,
		{1.6668195895748640911193660940626, -2.8034501918000431786026638292242, 17.366443344891639100069369305857},
		{0.088137253877983726124689667358325, 0.84367359658495044705261989292921, 0.52957217346105156519087131528067},
		{0.99407167458370515600307726344909, -0.040517375073519455719317505781873, -0.10089523332397798416337764138007}});
data.push_back({25,
		{6.7895112591572601701273015351035, -3.3559880789716265425681740453001, 16.812322223023997480595426168293},
		{0.19919120056925312955975471140846, 0.87871307456943525959758289900492, 0.43380433169403659343643653301115},
		{0.97743010728297419209553709151805, -0.14635765203147968471775186571904, -0.15234770450724138823872522152669}});
data.push_back({26,
		{12.361121448914440534849745745305, -4.6379357288872169462479178037029, 13.56393789734066324115246970905},
		{-0.044058356861438399687536104920582, 0.97872176814957612123180297203362, 0.20040599227776967339309521776158},
		{0.75378452327722467174453413463198, 0.16421740297360082339039877297182, -0.63627159061860782962583016342251}});
data.push_back({27,
		{16.827295713075379524070740444586, -3.4011801153122522833882612758316, 8.0801575084118102410002393298782},
		{0.015708419435684541770603672716788, 0.99211470131447776488187173526967, 0.12434494358242779987744341951839},
		{0.4549528933037146338769218800735, 0.10364920946412198299313445204461, -0.88446294792492097247560423056711}});
        return data;
    }

    void Test()
    {
        const vector_axis &axis = getAxisData();
        /*
        for (const auto& [id, origin, symmetry, angulation]: axis) {
            std::cout << "	{" << id << std::endl;
            std::cout << "	 {" << std::setprecision(32) << origin.x() << ", " << origin.data[1] << ", " << origin.z() << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << symmetry.x() << ", " << symmetry.data[1] << ", " << symmetry.z() << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << angulation.x() << ", " << angulation.data[1] << ", " << angulation.z() << "]" << std::endl;
        }
        */

        std::vector<unsigned char*> colors_lines;
        vtkNew<vtkNamedColors> namedColors;
        colors_lines.push_back(namedColors->GetColor3ub("yellow").GetData());
        colors_lines.push_back(namedColors->GetColor3ub("sea_green_light").GetData());
        colors_lines.push_back(namedColors->GetColor3ub("coral_light").GetData());


        vtkNew<vtkRenderer> renderer;
        auto actorPoint = vtkSmartPointer<vtkActor>::New();
		auto actorLines = vtkSmartPointer<vtkActor>::New();

        TeethAxisVis(axis,
                     0,
                     axis.size() - 1,
                     colors_lines,
                     namedColors->GetColor3d("cadmium_yellow").GetData(),
                     actorLines,
                     actorPoint);

        actorPoint->GetProperty()->SetPointSize(8);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();
        renderer->AddActor(actorPoint);
        renderer->AddActor(actorLines);


        actorPoint->SetVisibility(true);
		actorLines->SetVisibility(true);

		renderer->AddActor(actorPoint);
		renderer->AddActor(actorLines);
        renderer->SetBackground(namedColors->GetColor3d("DarkGray").GetData());

        vtkNew<vtkRenderWindow> window;
		window->SetSize(800, 800);
		window->SetPosition(0, 50);
		window->AddRenderer(renderer);
		window->SetWindowName("ReadSTL"); // TODO: Rename

		vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
		renderWindowInteractor->SetRenderWindow(window);

		window->Render();
		renderWindowInteractor->Start();

    }
}

#endif

namespace Axis::Tests {
    using namespace Types;

    using Point3D = Point<3, double>;

    struct Axis
    {
        int id = 0;                  //! id teeth
        Point<3, double>  origin;	 //! origin local
        Point<3, double>  symmetry;   //! Y local
        Point<3, double>  angulation; //! X local
    };


    void VisPoints(const double *pts,
                   const size_t nPoints,
                   double color_pt[3],
                   vtkSmartPointer<vtkActor> &actorPoints)
    {
        // Create the geometry of a point (the coordinate)
        auto points = vtkSmartPointer<vtkPoints>::New();

        for (size_t i = 0; i < nPoints; ++i) {
            points->InsertNextPoint(pts);
            pts += 3;
        }

        auto pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
        pointsPolydata->SetPoints(points);

        auto vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        // Create a polydata object
        auto polydata = vtkSmartPointer<vtkPolyData>::New();
        polydata->ShallowCopy(vertexFilter->GetOutput());
        // Visualize
        auto mapperPoints = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapperPoints->SetInputData(polydata);
        actorPoints->SetMapper(mapperPoints);
        actorPoints->GetProperty()->SetColor(color_pt);
    }


    bool TeethAxisVis(const std::vector<Axis>& axis,
                      ptrdiff_t n_start,
                      ptrdiff_t n_last,
                      const std::vector<unsigned char*> &colors_lines, // 3*(n_last + n_start) or size axis
                      double color_point[3],
                      vtkSmartPointer<vtkActor> &actorLines,
                      vtkSmartPointer<vtkActor> &actorPoints)
    {
        if (axis.empty() || n_start < 0)
            return false;
        const ptrdiff_t nAxis = axis.size();
        if (n_last >= nAxis)
            n_last = nAxis - 1;
        if (n_start > n_last)
            n_start = n_last;

        std::cout << "[DEBUG] " << __FILE__ << "." << __LINE__ << " [" << __FUNCTION__ << "] VisAxis: " << std::endl;
        for (const auto& [id, origin, symmetry, angulation]: axis) {
            std::cout << "	{" << id << std::endl;
            std::cout << "	origin    : [" << std::setprecision(32) << origin[0] << ", " << origin[1] << ", " << origin[2] << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << symmetry[0] << ", " << symmetry[1] << ", " << symmetry[2] << "]" << std::endl;
            std::cout << "	,{" << std::setprecision(32) << angulation[0] << ", " << angulation[1] << ", " << angulation[2] << "]" << std::endl;
        }



        std::vector<double> origin;
        const ptrdiff_t nOrigin = n_last - n_start + 1;
        origin.resize(nOrigin * 3);
        for (int i = n_start, j = 0; i <= n_last; ++i, j++) {
            origin[j++] = axis[i].origin[0];
            origin[j++] = axis[i].origin[1];
            origin[j] = axis[i].origin[2];
        }


        VisPoints(origin.data(), nOrigin, color_point, actorPoints);


        auto colorsArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colorsArray->SetNumberOfComponents(3);
        colorsArray->SetName("Colors");

        auto points = vtkSmartPointer<vtkPoints>::New();
        const ptrdiff_t nCount = (n_last - n_start + 1);

        Point3D z_l;
        Point3D pt;
        for (ptrdiff_t j = n_start; j <= n_last; ++j) {
            // Y local
            pt = axis[j].origin - axis[j].symmetry;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + axis[j].symmetry * 20; // 20 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[0]);


            // X local
            pt = axis[j].origin - axis[j].angulation;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + axis[j].angulation * 8; // 8 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[1]);


            // Z local
            z_l = cross(axis[j].angulation, axis[j].symmetry);
            pt = axis[j].origin - z_l;
            points->InsertNextPoint(pt.data());

            pt = axis[j].origin + z_l * 8; // 8 for size of line
            points->InsertNextPoint(pt.data());
            colorsArray->InsertNextTypedTuple(colors_lines[2]);
        }

        constexpr float _LINEWIDTH {1.0f};
        const ptrdiff_t nPoints = 3 * 2 * nCount;

        auto lines = vtkSmartPointer<vtkCellArray>::New();
        for (int i = 0; i < nPoints; i += 2) {
            auto line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
            lines->InsertNextCell(line);
        }
        auto polyDataLines = vtkSmartPointer<vtkPolyData>::New();
        polyDataLines->SetPoints(points);
        polyDataLines->SetLines(lines);
        polyDataLines->GetCellData()->SetScalars(colorsArray);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(polyDataLines);
        actorLines->GetProperty()->SetLineWidth(_LINEWIDTH);
        actorLines->SetMapper(mapper);

        return true;
    }


    std::vector<Axis> getAxisData() {
        std::vector<Axis> data;
        data.push_back({20,
                        {-17.896692992444137360052991425619, -3.9969198251673319077781343366951, 8.8359419949724493648091083741747},
                        {0.015708419435684541770603672716788, 0.99211470131447776488187173526967, 0.12434494358242779987744341951839},
                        {0.32071189044722075989213294633373, -0.12278889770548938320526843881453, 0.93918409799465885079428062454099}});
        data.push_back({21,
                        {-13.086340402933384297057273215614, -5.1170216201952669621277891565114, 12.942869366078982196199831378181},
                        {0.016073887605727189137327926005128, 0.9381718940605153767009483090078, 0.34579636685792941586115034624527},
                        {0.82564303944401362667093735581147, -0.20752101511455056659372075955616, 0.52464140105740553199353826130391}});
        data.push_back({22,
                        {-7.2028171979120481083214144746307, -3.8829669840949030401588970562443, 16.139603355482666557918491889723},
                        {-0.13985067314450275177684090976982, 0.81670376023907576001903407814098, 0.55985422855631261640496632026043},
                        {0.98197021122011285410025038800086, 0.041768535088715079417465148026167, 0.18436348269890570628071202463616}});
        data.push_back({23,
                        {-2.3483565298472459659251398988999, -2.8797274000887140843474298890214, 17.101444922953891847328122821636},
                        {-0.03808069627805529822062169387209, 0.69380705819800225775395574601134, 0.71915340961829032906393877055962},
                        {0.98109722481512640079870379850036, -0.11068543453301099566132847940025, 0.15873553490726644010777590665384}});
        data.push_back({24,
                        {1.6668195895748640911193660940626, -2.8034501918000431786026638292242, 17.366443344891639100069369305857},
                        {0.088137253877983726124689667358325, 0.84367359658495044705261989292921, 0.52957217346105156519087131528067},
                        {0.99407167458370515600307726344909, -0.040517375073519455719317505781873, -0.10089523332397798416337764138007}});
        data.push_back({25,
                        {6.7895112591572601701273015351035, -3.3559880789716265425681740453001, 16.812322223023997480595426168293},
                        {0.19919120056925312955975471140846, 0.87871307456943525959758289900492, 0.43380433169403659343643653301115},
                        {0.97743010728297419209553709151805, -0.14635765203147968471775186571904, -0.15234770450724138823872522152669}});
        data.push_back({26,
                        {12.361121448914440534849745745305, -4.6379357288872169462479178037029, 13.56393789734066324115246970905},
                        {-0.044058356861438399687536104920582, 0.97872176814957612123180297203362, 0.20040599227776967339309521776158},
                        {0.75378452327722467174453413463198, 0.16421740297360082339039877297182, -0.63627159061860782962583016342251}});
        data.push_back({27,
                        {16.827295713075379524070740444586, -3.4011801153122522833882612758316, 8.0801575084118102410002393298782},
                        {0.015708419435684541770603672716788, 0.99211470131447776488187173526967, 0.12434494358242779987744341951839},
                        {0.4549528933037146338769218800735, 0.10364920946412198299313445204461, -0.88446294792492097247560423056711}});
        return data;
    }


    void DrawAxis()
    {
        const std::vector<Axis>& axis = getAxisData();

        std::vector<unsigned char*> colors_lines;
        vtkNew<vtkNamedColors> namedColors;
        colors_lines.push_back(namedColors->GetColor3ub("yellow").GetData());
        colors_lines.push_back(namedColors->GetColor3ub("sea_green_light").GetData());
        colors_lines.push_back(namedColors->GetColor3ub("coral_light").GetData());

        vtkNew<vtkRenderer> renderer;
        auto actorPoint = vtkSmartPointer<vtkActor>::New();
        auto actorLines = vtkSmartPointer<vtkActor>::New();

        TeethAxisVis(axis,
                     0,
                     axis.size() - 1,
                     colors_lines,
                     namedColors->GetColor3d("cadmium_yellow").GetData(),
                     actorLines,
                     actorPoint);

        actorPoint->GetProperty()->SetPointSize(8);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();
        renderer->AddActor(actorPoint);
        renderer->AddActor(actorLines);

        actorPoint->SetVisibility(true);
        actorLines->SetVisibility(true);

        renderer->AddActor(actorPoint);
        renderer->AddActor(actorLines);
        renderer->SetBackground(namedColors->GetColor3d("Blue").GetData());

        vtkNew<vtkRenderWindow> window;
        window->SetSize(800, 800);
        window->SetPosition(0, 50);
        window->AddRenderer(renderer);
        window->SetWindowName("ReadSTL"); // TODO: Rename

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();

    }

    void DrawSimpleAxis()
    {
        vtkSmartPointer<vtkAxes> axes { vtkAxes::New() };
        axes->SetOrigin(0,0,0);
        axes->Update();

        Utilities::visualize(axes->GetOutput());
    }
}

void Axis::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    // Original::Test();

    // Tests::DrawAxis();

    Tests::DrawSimpleAxis();
}
