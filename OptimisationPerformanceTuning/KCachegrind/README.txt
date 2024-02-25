
export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64
export CC=gcc-13.1 CXX=g++-13.1

---------------------------------------------------------------------------------------------

# This is the simplest mode of operation

valgrind --tool=callgrind \
         --dump-instr=yes \
         --simulate-cache=yes \
         --collect-jumps=yes ./test_app \


valgrind --tool=callgrind --dump-instr=yes \
         --simulate-cache=yes \
         --collect-jumps=yes \
         --collect-atstart=no \
         --instr-atstart=no ./test_app \

---------------------------------------------------------------------------------------------
        After BUILD the Valgrind from sources
---------------------------------------------------------------------------------------------

export PATH=/home/andtokm/DiskS/Utils/valgrind:${PATH}

vg-in-place --tool=callgrind \
         --dump-instr=yes \
         --simulate-cache=yes \
         --collect-jumps=yes ./test_app \



-------------------------------------------------------------

1. after each valgrind 'callgrind.out.*' file will be created
2. then run 'kcachegrind' command

---------------------------------------------------------------------------------------------
        Memcheck
---------------------------------------------------------------------------------------------


vg-in-place --tool=memcheck  ./test_app
vg-in-place --tool=memcheck -v ./test_app