export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64
export CC=gcc-13.1 CXX=g++-13.1


# In order to build the application with settings for generating profiling information, we add the -pg flag.

g++-13.1 -O2 -pg *.cpp -o app


# Once the application, say app, is built, execute it as usual:
# This should produce a file called gmon.out.

./app


# To see the profiling results, now run

gprof app gmon.out


# translate profiling data to text, create image
gprof ./app | gprof2dot -s | dot -Tpng -o output.png.png