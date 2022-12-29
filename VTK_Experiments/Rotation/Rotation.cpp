//============================================================================
// Name        : Rotation.cpp
// Created on  : 17.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Rotation
//============================================================================

#include "Rotation.h"
#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"
#include <Core>

namespace Rotation::Utils {
    using namespace std::string_view_literals;
    using IdType = uint_fast16_t;

    static constexpr size_t MAX_TEETH_COUNT { 32 };
    static constexpr size_t MAX_TEETH_NUM_PER_JAW_SIDE { MAX_TEETH_COUNT / 2 };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the upper jaw according to the FDI notation
    static constexpr std::array<IdType, MAX_TEETH_NUM_PER_JAW_SIDE> upperTeeth {
            18,  17,  16,  15,  14,  13,  12,  11,
            21,  22,  23,  24,  25,  26,  27,  28
    };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the lower jaw according to the FDI notation
    static constexpr std::array<IdType, MAX_TEETH_NUM_PER_JAW_SIDE> lowerTeeth {
            48,  47,  46,  45,  44,  43,  42,  41,
            31,  32,  33,  34,  35,  36,  37,  38
    };

    static constexpr std::array<IdType, MAX_TEETH_COUNT> teethIds {
            18,  17,  16,  15,  14,  13,  12,  11, 21,  22,  23,  24,  25,  26,  27,  28,
            48,  47,  46,  45,  44,  43,  42,  41, 31,  32,  33,  34,  35,  36,  37,  38
    };

    // TODO: Do we need it
    static constexpr std::array<std::string_view, MAX_TEETH_COUNT> teethIdsStr {
            "18"sv,  "17"sv,  "16"sv,  "15"sv, "14"sv,  "13"sv,  "12"sv,  "11"sv,
            "21"sv,  "22"sv,  "23"sv,  "24"sv, "25"sv,  "26"sv,  "27"sv,  "28"sv,
            "48"sv,  "47"sv,  "46"sv,  "45"sv, "44"sv,  "43"sv,  "42"sv,  "41"sv,
            "31"sv,  "32"sv,  "33"sv,  "34"sv, "35"sv,  "36"sv,  "37"sv,  "38"sv
    };

    static constexpr IdType MAX_LOWER_JAW_TOOTH =
            *std::max_element(lowerTeeth.begin(), lowerTeeth.end());

    static constexpr IdType MIN_LOWER_JAW_TOOTH =
            *std::min_element(lowerTeeth.begin(), lowerTeeth.end());

    static constexpr IdType MAX_UPPER_JAW_TOOTH =
            *std::max_element(upperTeeth.begin(), upperTeeth.end());

    static constexpr IdType MIN_UPPER_JAW_TOOTH =
            *std::min_element(upperTeeth.begin(), upperTeeth.end());

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isLowerTooth(int id) noexcept {
        return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isUpperTooth(int id) noexcept {
        return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isLeftSideTooth(int id) noexcept {
        return (id >= 21 && id <= 28) || (id >= 31 && id <= 38);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isRightSideTooth(int id) noexcept {
        return (id >= 11 && id <= 28) || (id >= 41 && id <= 48);
    }


    // TODO: refactor
    Eigen::Vector3d toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Eigen::Vector3d pt { Eigen::Vector3d::Zero () };
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
        std::vector<Eigen::Vector3d> points;
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

                // TODO: Check for MEM LEAK
                vtkSmartPointer<vtkPoints> vtkPoints = vtkPoints::New();
                for (const Eigen::Vector3d& pt: points)
                    vtkPoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                const int pointsCount { static_cast<int>(points.size())};
                vtkSmartPointer<vtkCellArray> vtkCells { vtkSmartPointer<vtkCellArray>::New() };
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

    // TODO: Move to Utilities ?????
    [[nodiscard]]
    constexpr double degToRad(const double angle) noexcept {
        return angle * std::numbers::pi / 180;
    };
}


namespace Rotation {
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };


    void EigenRotationMatrix() {
        const vtkSmartPointer<vtkTransform> transform{vtkTransform::New()};
        transform->RotateX(0);
        transform->RotateY(15);
        transform->RotateZ(76);

        vtkSmartPointer<vtkMatrix3x3> matrix = Utils::cast4x4MatrixTo3x3(transform->GetMatrix());
        std::cout << *matrix << std::endl;

        Eigen::Matrix3d rotationMatrix {matrix->GetData()};
        std::cout << rotationMatrix.transpose() << std::endl;
    }


    void Rotate_Tooth() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_13.stl)"};
        vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);


        {
            polyData = Utilities::rotatePolyData(polyData, 0, 80, 0);
            Utilities::visualize(polyData);
        }


        {
            constexpr float yAngle = -90;
            const double sinY = std::sin(Utils::degToRad(yAngle)),
                         cosY = std::cos(Utils::degToRad(yAngle));
            const Eigen::Matrix3d matrix {
                    { cosY, 0,  sinY },
                    { 0,    1,     0 },
                    {-sinY, 0,  cosY }
            };

            vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
            polyData = Utilities::rotatePolyData(polyData, vtkMatrix);

            Utilities::visualize(polyData);

            {
                polyData = Utilities::rotatePolyData(polyData, -10, 0, 0);
            }

            vtkMatrix->Transpose();
            polyData = Utilities::rotatePolyData(polyData, vtkMatrix);

            Utilities::visualize(polyData);
        }
    }

    void Rotate_SingleTooth() {
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2494/automodeling/out/2494_teeth.obj)"};
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap
                = Utils::ReadTeethObj(filePath);

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, data]: toothMap) {
            if (Utils::isLowerTooth(id)) {
                vtkSmartPointer<vtkPolyData> pd (data);
                if (31 == id)
                {
                    Eigen::Vector3d pos {};
                    pd->GetCenter(pos.data());
                    pd = Utilities::setPolyDataCenter(pd, 0, 0, 0);
                    pd = Utilities::rotatePolyData(pd, 0, 90, 0);
                    pd = Utilities::setPolyDataCenter(pd, pos.x(), pos.y(), pos.z());
                }
                const auto actor = Utilities::getPolyDataActor(pd);
                actors.push_back(actor);
            }
        }
        Utilities::DisplayActors(actors);
    }
};

void Rotation::TestAll()
{
    // Rotate_Tooth();
    Rotate_SingleTooth();
};