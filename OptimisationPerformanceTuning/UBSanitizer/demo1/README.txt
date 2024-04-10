
export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64

make

=================================================================================================

gcc -o ./main ./test.c -fsanitize=address


=================================================================================================
                        Output of  ./test_bin
=================================================================================================
