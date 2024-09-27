
export GCC_VERSION=14.2
export UTILS_PATH=/home/andtokm/DiskS/Utils
export GCC_PATH=${UTILS_PATH}/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:${UTILS_PATH}/cmake/cmake-3.30/bin/:${PATH}

export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

make


=================================================================================================

=================================================================================================

# https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html

=================================================================================================
                            Output
=================================================================================================



main.cpp:7:17: runtime error: division by zero
AddressSanitizer:DEADLYSIGNAL
=================================================================
==33069==ERROR: AddressSanitizer: FPE on unknown address 0x00000040119f (pc 0x00000040119f bp 0x7ffde8684aa0 sp 0x7ffde8684a70 T0)
    #0 0x40119f in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/UBSanitizer/divide_by_zero/test_bin+0x40119f)
    #1 0x7f028522e082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010ad in _start (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/UBSanitizer/divide_by_zero/test_bin+0x4010ad)

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: FPE (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/UBSanitizer/divide_by_zero/test_bin+0x40119f) in main
==33069==ABORTING
