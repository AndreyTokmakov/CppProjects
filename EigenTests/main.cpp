
#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>

#include <thread>
#include <future>

#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <iomanip>

#include <Core>
#include <Geometry> 

#include "Arrays/Arrays.h"
#include "Vectors/Vectors.h"
#include "Matrices/Matrices.h"
#include "Quaternion/Quaternion.h"
#include "LeastSquares/LeastSquares.h"

struct Info
{
    const static inline int info = [] {
        std::cout << "Using Eigen version : " << EIGEN_MAJOR_VERSION << "."
                  << EIGEN_MINOR_VERSION << std::endl << std::endl;
        return 0;
    }();
};




int main([[maybe_unused]] int argc, 
         [[maybe_unused]] char** argv) 
{
    Matrices::TestAll();
    // Vectors::TestAll();
    // Quaternion::TestAll();
    // Arrays::TestAll();
    // LeastSquares::TestAll();
}