---------------------------------------------------------------------------------------------

export GCC_VERSION=13.2
export GCC_PATH=/home/andtokm/DiskS/Utils/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:/home/andtokm/DiskS/Utils/cmake/cmake-3.25.1/bin/:${PATH}
export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

make

---------------------------------------------------------------------------------------------
        Install and configure
---------------------------------------------------------------------------------------------

cd DiskS/ProjectsUbuntu/third_party
git clone git@github.com:gperftools/gperftools.git
cmake ..
make -j8


---------------------------------------------------------------------------------------------



CPUPROFILE=prof.data CPUPROFILE_FREQUENCY=1000 \
LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.2/lib64:/home/andtokm/DiskS/ProjectsUbuntu/third_party/gperftools/build \
./test_app



/home/andtokm/DiskS/ProjectsUbuntu/third_party/gperftools/build/google-pprof ./test_app prof.data

google-pprof ./test_app prof.data