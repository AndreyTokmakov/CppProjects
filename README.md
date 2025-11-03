# CppProjects
Cpp project to synchronize C++ code base across multiple devieses 


====================================================================================
            C++ | GCC | CMAKE
====================================================================================

export GCC_VERSION=14.2
export UTILS_PATH=/home/andtokm/DiskS/Utils
export GCC_PATH=${UTILS_PATH}/bin/gcc-$GCC_VERSION

export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

export PATH=${GCC_PATH}/bin::${PATH}
export PATH=${GCC_PATH}/bin:${UTILS_PATH}/cmake/cmake-3.29.5/bin/:${PATH}

cmake -DCMAKE_BUILD_TYPE=Release -B./build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc-14.2 -DCMAKE_CXX_COMPILER=g++-14.2 -DCMAKE_CXX_STANDARD=23 -B./build

====================================================================================
            Libs
====================================================================================

sudo apt-get install build-essential cmake make libboost-all-dev libpcap-dev


====================================================================================
            Third-party
====================================================================================

cd ..../third_party

git clone git@github.com:catchorg/Catch2.git
git clone git@github.com:libssh2/libssh2.git
git clone git@github.com:open-source-parsers/jsoncpp.git
git clone git@github.com:Tencent/rapidjson.git
git clone git@github.com:nlohmann/json.git
git clone git@github.com:crayzeewulf/libserial.git
git clone git@github.com:odygrd/quill.git
git clone git@github.com:grpc/grpc.git
git clone git@github.com:abseil/abseil-cpp.git
git clone git@github.com:kovacsnador/tinycoro.git
git clone https://gitlab.com/libeigen/eigen.git
git clone https://github.com/gabime/spdlog.git
git clone https://github.com/weidai11/cryptopp
git clone https://github.com/ClickHouse/clickhouse-cpp.git
git clone git@github.com:jpbarrette/curlpp.git


====================================================================================
                    Disable Optimisation - 1
====================================================================================


#pragma GCC push_options
#pragma GCC optimize("O0")

    void someTestFunction()
    {
        // SOME CODE TO TEST
    }

#pragma GCC pop_options


====================================================================================
                    Disable Optimisation - 2
====================================================================================


    __attribute__((optimize("O0")))
    void someTestFunction()
    {
        // SOME CODE TO TEST
    }

====================================================================================