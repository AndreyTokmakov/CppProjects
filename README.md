# CppProjects
Cpp project to synchronize C++ code base across multiple devieses 


====================================================================================
            C++ | GCC | CMAKE
====================================================================================

export GCC_13_PATH=/home/andtokm/Utils/bin/gcc-13.2/
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_13_PATH}/lib64
export CC=gcc-13.2 CXX=g++-13.2

cmake -DCMAKE_BUILD_TYPE=Release ..

====================================================================================
            Libs
====================================================================================

sudo apt install libpcap-dev


====================================================================================
            Third-party
====================================================================================

cd ..../third_party

git clone git@github.com:catchorg/Catch2.git
git clone https://gitlab.com/libeigen/eigen.git
git clone git@github.com:libssh2/libssh2.git
git clone git@github.com:open-source-parsers/jsoncpp.git
git clone git@github.com:Tencent/rapidjson.git
git clone git@github.com:crayzeewulf/libserial.git