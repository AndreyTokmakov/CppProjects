# CppProjects
Cpp project to synchronize C++ code base across multiple devieses 


====================================================================================
            C++ | GCC | CMAKE
====================================================================================

export GCC_VERSION=13.2
export GCC_PATH=/home/andtokm/DiskS/Utils/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release ..

====================================================================================
            Libs
====================================================================================

sudo apt-get install build-essential cmake make libboost-all-dev libpcap-dev

====================================================================================
            Third-party
====================================================================================

cd ..../third_party

git clone https://gitlab.com/libeigen/eigen.git
git clone git@github.com:catchorg/Catch2.git
git clone git@github.com:libssh2/libssh2.git
git clone git@github.com:open-source-parsers/jsoncpp.git
git clone git@github.com:Tencent/rapidjson.git
git clone git@github.com:nlohmann/json.git
git clone git@github.com:crayzeewulf/libserial.git
git clone git@github.com:odygrd/quill.git
git clone git@github.com:grpc/grpc.git